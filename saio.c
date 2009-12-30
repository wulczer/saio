/*------------------------------------------------------------------------
 *
 * saio.c
 *	  solution to the query optimization problem
 *	  by means of Simulated Annealing (SA)
 *
 * Copyright (c) 2009, PostgreSQL Global Development Group
 *
 * $PostgreSQL$
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "nodes/pg_list.h"
#include "optimizer/paths.h"
#include "optimizer/pathnode.h"
#include "optimizer/joininfo.h"

#include "saio_debug.h"
#include "saio.h"


/*
 * Initialize the context under which join order. It will keep the state of
 * variables that get modified during planning and have to be reset to their
 * original state when the joining ends;
 *
 * It also holds a sketch memory context that will hold all memory allocations
 * done during considering a given join order. Since we are going to consider
 * lots of them, we need to free the memory after each try.
 *
 * See geqo_eval() for similar code and explanations.
 */
static void
context_init(SAIOJoinOrderContext *ctx)
{
	ctx->sketch_context = AllocSetContextCreate(CurrentMemoryContext,
												"SAIO",
												ALLOCSET_DEFAULT_MINSIZE,
												ALLOCSET_DEFAULT_INITSIZE,
												ALLOCSET_DEFAULT_MAXSIZE);
}


/*
 * Save the current state of the variables that get modified during
 * make_join_rel(). Enter a temporary memory context that will get reset when
 * we leave the context.
 */
static void
context_enter(PlannerInfo *root, SAIOJoinOrderContext *ctx)
{
	/* join_rel_list and join_rel_hash get added to in make_join_rel() */
	ctx->savelength = list_length(root->join_rel_list);
	ctx->savehash = root->join_rel_hash;
	/* if a hash has already been built, we need to get rid of it */
	root->join_rel_hash = NULL;

	/* switch to the sketch context */
	ctx->old_context = MemoryContextSwitchTo(ctx->sketch_context);
}


/*
 * Restore the state and reclaim memory.
 */
static void
context_exit(PlannerInfo *root, SAIOJoinOrderContext *ctx)
{
	/* restore join_rel_list and join_rel_hash */
	root->join_rel_list = list_truncate(root->join_rel_list,
										ctx->savelength);
	root->join_rel_hash = ctx->savehash;

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(ctx->sketch_context);
	/* switch back to the old context */
	MemoryContextSwitchTo(ctx->old_context);
}


static List *
list_intersection_ptr(List *list1, List *list2)
{
	List	   *result;
	ListCell   *cell;

	if (list1 == NIL || list2 == NIL)
		return NIL;

	result = NIL;
	foreach(cell, list1)
	{
		if (list_member_ptr(list2, lfirst(cell)))
			result = lappend(result, lfirst(cell));
	}

	return result;
}


/* verbatim copy from geqo_eval.c */
static bool
desirable_join(PlannerInfo *root,
			   RelOptInfo *outer_rel, RelOptInfo *inner_rel)
{
	/*
	 * Join if there is an applicable join clause, or if there is a join order
	 * restriction forcing these rels to be joined.
	 */
	if (have_relevant_joinclause(root, outer_rel, inner_rel) ||
		have_join_order_restriction(root, outer_rel, inner_rel))
		return true;

	/* Otherwise postpone the join till later. */
	return false;
}


/*
 * Merge a QueryTree into a list of QueryTrees by creating another tree that
 * will have the given one as one child and one of the existing ones as the
 * other child.
 *
 * Only considers creating joins that have join conditions, unless the force
 * parameter is true. Based heavily on GEQO's merge_clump(), but does not
 * maintain the invariant of having the list sorted by the size of the tree.
 *
 * Returns a new list of QueryTrees.
 */
static List *
merge_trees(PlannerInfo *root, List *result, QueryTree *tree, bool force)
{
	ListCell	*prev;
	ListCell	*lc;

	/* Go through the trees and find one to join the new one */
	prev = NULL;
	foreach(lc, result)
	{
		QueryTree	*other_tree = (QueryTree *) lfirst(lc);

		/* only choose ones that form a "desirable" join, unless forced */
		if (force || desirable_join(root, other_tree->rel, tree->rel))
		{
			RelOptInfo	*joinrel;

			/*
			 * Try joining the relations. This can fail because of join order
			 * restrictions or other restrictions.
			 */
			joinrel = make_join_rel(root, other_tree->rel, tree->rel);

			if (joinrel)
			{
				QueryTree	*new_tree;

				/* Managed to construct the join, build a new QueryTree */
				new_tree = (QueryTree *) palloc0(sizeof(QueryTree));

				/*
				 * The new tree's rel will be the relation that's just been
				 * built, and the children will be the just-merged tree and the
				 * one we took the other relation from.
				 * It doesn't matter which one is right and which one is left,
				 * because make_join_rel is symmetrical.
				 */
				new_tree->rel = joinrel;
				new_tree->left = other_tree;
				new_tree->right = tree;

				/* set the parent link, too */
				other_tree->parent = new_tree;
				tree->parent = new_tree;

				/* Find the cheapest path for the new relation */
				set_cheapest(joinrel);

				/* Remove the old tree from the list */
				result = list_delete_cell(result, lc, prev);

				/* Recursively merge the new tree into the list */
				return merge_trees(root, result, new_tree, force);
			}
		}
		prev = lc;
	}

	/* We don't care about the ordering, just add it at the beginning */
	return lcons(tree, result);
}


/*
 * Build a QueryTree from a list of RelOptInfos. The tree structure will
 * reflect the order in which the rels are joined.
 *
 * Heavily based on GEQO's gimme_tree(). Note however, that it gets called only
 * once in one SAIO algorithm execution.
 */
static QueryTree *
make_query_tree(PlannerInfo *root, List *initial_rels)
{
	List		*result;
	ListCell	*lc;

	result = NIL;

	foreach(lc, initial_rels)
	{
		/* Get the next relation */
		RelOptInfo	*rel = (RelOptInfo *) lfirst(lc);
		QueryTree	*tree;

		/* Make it into a one-level tree */
		tree = (QueryTree *) palloc0(sizeof(QueryTree));
		tree->rel = rel;

		/* Merge it into the trees list using only desirable joins */
		result = merge_trees(root, result, tree, false);
	}

	if (list_length(result) > 1)
	{
		/* Force-join the remaining trees in some legal order */
		List	   *fresult;
		ListCell   *flc;

		fresult = NIL;
		foreach(flc, result)
		{
			QueryTree	*tree = (QueryTree *) lfirst(flc);

			fresult = merge_trees(root, fresult, tree, true);
		}
		result = fresult;
	}

	/* Did we succeed in forming a single join relation? */
	if (list_length(result) != 1)
		elog(ERROR, "failed to join all relations together");

	return (QueryTree *) linitial(result);
}


/* Recursive helper for get_tree_subtree() */
static List *
get_all_nodes_rec(QueryTree *tree, List *res)
{
	if (tree == NULL)
		return res;

	res = lcons(tree, res);
	get_all_nodes_rec(tree->left, res);
	get_all_nodes_rec(tree->right, res);
	return res;
}


/*
 * Return a list of all nodes of in a tree.
 */
static List *
get_all_nodes(QueryTree *tree)
{
	return get_all_node_rec(tree, NIL);
}


/* Recursive helper for get_parents */
static List *
get_parents_rec(QueryTree *tree, bool reverse, List *res)
{
	Assert(tree != NULL);

	while (tree->parent != NULL)
	{
		if (reverse)
			res = lappend(res, tree->parent);
		else
			res = lcons(tree->parent, res);
		tree = tree->parent;
	}
	return res;
}


/*
 * Get all parents of a tree node, excluding the node itself.
 */
static List *
get_parents(QueryTree *tree, bool reverse)
{
	return get_parents_rec(tree, reverse, NIL);
}


/*
 * Get all siblings of a tree node. Since QueryTrees are binary, this will
 * always be a one-element list.
 */
static List *
get_siblings(QueryTree *tree)
{
	QueryTree	*parent;

	Assert(tree != NULL);

	parent = tree->parent;

	if (parent == NULL)
		return NIL;

	Assert(parent->left == tree || parent->right == tree);

	if (parent->left == tree)
		return list_make1(parent->right);

	/* has to be the left child */
	return list_make1(parent->left);
}


/*
 * recalculate_tree
 *    Rebuild the final relation in the order given by the input tree.
 *
 * Traverses the QueryTree in postorder and recreates upper-level joinrels from
 * lower-level joinrels. The assumption is that leaf nodes have the XXX
 */
static bool
recalculate_tree(PlannerInfo *root, QueryTree *tree)
{
	RelOptInfo	*joinrel;
	bool		ok;

	Assert(tree != NULL);
	Assert(tree->right != NULL);
	Assert(tree->right != NULL);

	ok = true;

	if (tree->left->rel == NULL)
		ok = recalculate_tree(root, tree->left);

	Assert(!ok || (tree->left->rel != NULL));

	if (!ok)
		return false;

	if (tree->right->rel == NULL)
		ok = recalculate_tree(root, tree->right);

	Assert(!ok || (tree->left->rel != NULL));

	if (!ok)
		return false;

	debug_printf("Recalculating: left = (");
	fprintf_relids(stdout, tree->left->rel->relids);
	debug_printf(") right = (");
	fprintf_relids(stdout, tree->right->rel->relids);
	debug_printf(")\n");

	joinrel = make_join_rel(root, tree->left->rel, tree->right->rel);
	if (joinrel == NULL)
		return false;

	trace_join("/tmp/trace-rebuild", tree->left->rel, tree->right->rel);

	set_cheapest(joinrel);
	debug_printf("Built joinrel for (");
	fprintf_relids(stdout, joinrel->relids);
	debug_printf(") with cheapest path is %.2f\n", joinrel->cheapest_total_path->total_cost);

	tree->rel = joinrel;

	return true;
}


static void
cleanup_tree(QueryTree *tree)
{
	if (tree == NULL)
		return;

	if (tree->left != NULL)
	{
		Assert(tree->right != NULL);
		tree->rel = NULL;
	}
	else
	{
		Assert(tree->right == NULL);
	}
	cleanup_tree(tree->left);
	cleanup_tree(tree->right);
}


static void
swap_subtrees(QueryTree *tree1, QueryTree *tree2)
{
	QueryTree	*parent1, *parent2;

	Assert(tree1->parent != NULL);
	Assert(tree2->parent != NULL);

	parent1 = tree1->parent;
	parent2 = tree2->parent;

	Assert(parent1 != parent2);

	if (parent1->left == tree1)
		parent1->left = tree2;
	else
		parent1->right = tree2;

	if (parent2->left == tree2)
		parent2->left = tree1;
	else
		parent2->right = tree1;

	tree1->parent = parent2;
	tree2->parent = parent1;
}


static Cost
do_move(PlannerInfo *root, QueryTree *tree,
		QueryTree *tree1, QueryTree *tree2,
		int loops, Cost current_cost)
{
	List		*t1, *t2, *common;
	bool 		ok;

	char dump_before[NAMEDATALEN];
	char dump_middle[NAMEDATALEN];
	char dump_after[NAMEDATALEN];

	int			savelength;
	struct HTAB *savehash;

	snprintf(dump_before, NAMEDATALEN, "/tmp/before-move-%d.dot", loops);
	snprintf(dump_middle, NAMEDATALEN, "/tmp/middle-move-%d.dot", loops);
	snprintf(dump_after, NAMEDATALEN, "/tmp/after-move-%d.dot", loops);

	cleanup_tree(tree);

	dump_query_tree(root, tree, tree1, tree2, dump_before);

	swap_subtrees(tree1, tree2);

	t1 = get_parents(tree1, true);
	t2 = get_parents(tree2, true);
	common = list_intersection_ptr(t1, t2);

	t1 = list_difference_ptr(t1, common);

	savelength = list_length(root->join_rel_list);
	savehash = root->join_rel_hash;
	root->join_rel_hash = NULL;
	Assert(root->join_rel_level == NULL);

	ok = recalculate_tree(root, tree);

	root->join_rel_list = list_truncate(root->join_rel_list,
										savelength);
	root->join_rel_hash = savehash;

	dump_query_tree(root, tree, tree1, tree2, dump_middle);

	if (ok)
	{
		Assert(tree->rel != NULL);
		debug_printf("Cost after move: %.2f\n", tree->rel->cheapest_total_path->total_cost);
		ok = (tree->rel->cheapest_total_path->total_cost < current_cost);
		if (ok)
			elog(NOTICE, "Found a better plan, old cost %.2f new cost %.2f",
				 current_cost, tree->rel->cheapest_total_path->total_cost);

	}

	debug_printf("Keeping the state: %d\n", ok);

	if (!ok)
	{
		swap_subtrees(tree2, tree1);
		cleanup_tree(tree);

		savelength = list_length(root->join_rel_list);
		savehash = root->join_rel_hash;
		root->join_rel_hash = NULL;

		recalculate_tree(root, tree);

		root->join_rel_list = list_truncate(root->join_rel_list,
											savelength);
		root->join_rel_hash = savehash;
	}

	dump_query_tree(root, tree, tree1, tree2, dump_after);

	return ok ? tree->rel->cheapest_total_path->total_cost : current_cost;
}


static void
saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	Cost		current_cost;
	QueryTree	*tree1, *tree2;
	int			loops;
	SAIOPrivate	*private;
	Cost		*ccost;

	if (list_length(all_trees) == 1)
		return;

	loops = 0;

	print_query_tree_list("All trees: ", all_trees);

	Assert(tree->rel != NULL);
	current_cost = tree->rel->cheapest_total_path->total_cost;

	private = (SAIOPrivate *) root->join_search_private;
	ccost = (Cost *) palloc(sizeof(Cost));
	*ccost = current_cost;
	private->costs = lappend(private->costs, ccost);

	while (loops++ < saio_cutoff)
	{

		debug_printf("Loop %d, current cost cost: %.2f\n", loops, current_cost);
		choices = list_copy(all_trees);
		choices = list_delete_ptr(choices, llast(choices));

		print_query_tree_list("Choices for first node: ", choices);

		tree1 = list_nth(choices, random() % list_length(choices));

		print_query_tree_list("First node: ", list_make1(tree1));

		tmp = get_tree_subtree(tree1);
		tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
		tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
		choices = list_difference_ptr(choices, tmp);

		print_query_tree_list("Choices for second node: ", choices);

		if (choices == NIL)
			continue;

		tree2 = list_nth(choices, random() % list_length(choices));

		print_query_tree_list("Second node: ", list_make1(tree2));

		debug_printf("Move starting\n");
		current_cost = do_move(root, tree, tree1, tree2, loops, current_cost);

		private = (SAIOPrivate *) root->join_search_private;
		ccost = (Cost *) palloc(sizeof(Cost));
		*ccost = current_cost;
		private->costs = lappend(private->costs, ccost);

		debug_printf("Move finished\n");
	}
}


RelOptInfo *
saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	RelOptInfo	*res;
	QueryTree	*tree;
	List		*all_trees;
	int			savelength;
	struct HTAB *savehash;
	SAIOPrivate	*private;

	private = palloc0(sizeof(SAIOPrivate));
	private->costs = NIL;

	root->join_search_private = (void *) private;

	savelength = list_length(root->join_rel_list);
	savehash = root->join_rel_hash;
	Assert(root->join_rel_level == NULL);
	root->join_rel_hash = NULL;

	tree = make_query_tree(root, initial_rels);

	root->join_rel_list = list_truncate(root->join_rel_list,
										savelength);
	root->join_rel_hash = savehash;

	Assert(tree->rel != NULL);

	private->temperature = tree->rel->cheapest_total_path->total_cost * 2;

	all_trees = get_tree_subtree(tree);
	dump_query_tree(root, tree, NULL, NULL, "/tmp/original.dot");

	saio_move(root, tree, all_trees);

	cleanup_tree(tree);
	recalculate_tree(root, tree);

	Assert(tree->rel != NULL);

	dump_query_tree(root, tree, NULL, NULL, "/tmp/final.dot");
	dump_costs(root, "/tmp/costs");

	res = tree->rel;

	return res;
}
