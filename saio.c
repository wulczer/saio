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

#include <math.h>

#include "utils/memutils.h"
#include "nodes/pg_list.h"
#include "optimizer/paths.h"
#include "optimizer/pathnode.h"
#include "optimizer/joininfo.h"

#include "saio_debug.h"
#include "saio.h"


#define SAIO_COST(rel) rel->cheapest_total_path->total_cost


/*
 * Save the current state of the variables that get modified during
 * make_join_rel(). Enter a temporary memory context that will get reset when
 * we leave the context.
 */
static void
context_enter(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* join_rel_list and join_rel_hash get added to in make_join_rel() */
	private->savelength = list_length(root->join_rel_list);
	private->savehash = root->join_rel_hash;
	/* if a hash has already been built, we need to get rid of it */
	root->join_rel_hash = NULL;

	/* switch to the sketch context */
	private->old_context = MemoryContextSwitchTo(private->sketch_context);
}


/*
 * Restore the state, reset the sketch memory context.
 */
static void
context_exit(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* restore join_rel_list and join_rel_hash */
	root->join_rel_list = list_truncate(root->join_rel_list,
										private->savelength);
	root->join_rel_hash = private->savehash;

	/* switch back to the old context */
	MemoryContextSwitchTo(private->old_context);

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(private->sketch_context);
}

/*
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
*/

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
				new_tree = (QueryTree *) palloc(sizeof(QueryTree));

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
				new_tree->parent = NULL;

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


/* Recursive helper for get_all_nodes_rec() */
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
	return get_all_nodes_rec(tree, NIL);
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
 * Copy a query tree ignoring non-leaf rels. Used to transfer a tree to a
 * different memory context.
 */
static QueryTree *
copy_tree_structure(QueryTree *tree)
{
	QueryTree *res;

	/* copying an empty tree is easy */
	if (tree == NULL)
		return NULL;

	/* tree is nonempty, create the current node */
	res = (QueryTree *) palloc(sizeof(QueryTree));

	/*
	 * Set the parent to NULL, for all nodes except the root node, it will be
	 * then set correctly by the higher-level recursive invocation.
	 */
	res->parent = NULL;

	/* for leaf nodes, copy the rel */
	if (tree->left == NULL)
		res->rel = tree->rel;

	/* copy the children */
	res->right = copy_tree_structure(tree->right);
	res->left = copy_tree_structure(tree->left);

	/* set the parent */
	if (res->right != NULL)
	{
		Assert(res->left != NULL);
		res->left->parent = res;
		res->right->parent = res;
	}

	return res;
}


/*
 * recalculate_tree
 *    Rebuild the final relation in the order given by the input tree.
 *
 * Traverses the QueryTree in postorder and recreates upper-level joinrels from
 * lower-level joinrels, starting at the leaves.
 *
 * If any of the intermidient joinrels cannot be constructed (because the tree
 * does not represent a valid join ordering) return false, otherwise return
 * true.
 *
 * If this function returns false, the tree is left in a dirty state, where the
 * values of "rel" pointers in the non-leaf nodes can contain bogus values.
 */
static bool
recalculate_tree(PlannerInfo *root, QueryTree *tree)
{
	RelOptInfo	*joinrel;
	bool		ok;

	/* we should never be called on an empty tree */
	Assert(tree != NULL);

	/* if it's a leaf, we're done */
	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		return true;
	}

	/* it's not a leaf */
	Assert(tree->right != NULL);

	/* recurse to the left child */
	ok = recalculate_tree(root, tree->left);

	/* it either failed or computed the left child's rel */
	Assert(!ok || (tree->left->rel != NULL));

	/* short-circuit the computation on failure*/
	if (!ok)
		return false;

	/* recurse to the right child */
	ok = recalculate_tree(root, tree->right);

	/* it either failed or computed the left child's rel */
	Assert(!ok || (tree->left->rel != NULL));

	/* short-circuit the computation on failure*/
	if (!ok)
		return false;

	/* try to join the children's relations */
	joinrel = make_join_rel(root, tree->left->rel, tree->right->rel);

	if (joinrel)
	{
		/* constructed the joinrel, compute its paths and store it */
		set_cheapest(joinrel);
		tree->rel = joinrel;
		return true;
	}

	/* failed to build the joinrel */
	return false;
}



/* Swap two subtrees around. */

static void
swap_subtrees(QueryTree *tree1, QueryTree *tree2)
{
	QueryTree	*parent1, *parent2;

	/* can't swap the root node with anything */
	Assert(tree1->parent != NULL);
	Assert(tree2->parent != NULL);

	/*
	 * Since make_join_rel() is symmetrical, it doesn't make sense to swap
	 * sibling nodes.
	 */
	Assert(tree1->parent != tree2->parent);

	/* Do the swap */
	parent1 = tree1->parent;
	parent2 = tree2->parent;

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


static double
saio_rand(PlannerInfo *root)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	return erand48(private->random_state);
}


static bool
acceptable(PlannerInfo *root, Cost new_cost)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	/* downhill moves are always acceptable */
	if (new_cost < private->previous_cost)
		return true;

	/*
	 * Uphill moves are acceptable with probability
	 *  exp((new - old) / temperature)
	 */
	return (saio_rand(root) < exp(
				((double) (new_cost - private->previous_cost))
				/ private->temperature));
}


static bool
saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	QueryTree	*tree1, *tree2;
	bool		ok;
	SaioPrivateData	*private;

	/* only one tree to choose from, return immediately */
	if (list_length(all_trees) == 1)
		return false;

	private = (SaioPrivateData *) root->join_search_private;

	context_enter(root);

	choices = list_copy(all_trees);
	choices = list_delete_ptr(choices, llast(choices));

	tree1 = list_nth(choices, random() % list_length(choices));

	tmp = get_all_nodes(tree1);
	tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
	tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
	choices = list_difference_ptr(choices, tmp);

	if (choices == NIL)
	{
		context_exit(root);
		return false;
	}

	tree2 = list_nth(choices, random() % list_length(choices));

	swap_subtrees(tree1, tree2);

	ok = recalculate_tree(root, tree);
	if (ok)
		ok = acceptable(root, SAIO_COST(tree->rel));

	if (!ok)
		swap_subtrees(tree1, tree2);
	else
		private->previous_cost = SAIO_COST(tree->rel);

	context_exit(root);
	return ok;
}


static bool
equilibrium(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->elapsed_loops++;

	if (private->elapsed_loops >= private->equilibrium_loops)
	{
		/* reset the elapsed loops count */
		private->elapsed_loops = 0;
		/* equilibrium reached */
		return true;
	}

	return false;
}


static void
reduce_temperature(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->temperature *= saio_temperature_reduction_factor;
}


static bool
frozen(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* can only be frozen when temperature < 1 */
	if (private->temperature > 1)
		return false;

	/* check the number of consecutive failed moves */
	return private->failed_moves >= saio_moves_before_frozen;
}


/* verbatim copy from geqo_random.c */
static void
initialize_random_state(PlannerInfo *root, double seed)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	memset(private->random_state, 0, sizeof(private->random_state));
	memcpy(private->random_state,
		   &seed,
		   Min(sizeof(private->random_state), sizeof(seed)));
}


RelOptInfo *
saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	QueryTree		*tree;
	List			*all_trees;
	SaioPrivateData	private;

	/* Initialize private data */
	root->join_search_private = (void *) &private;

	/* Initialize the random state */
	initialize_random_state(root, saio_seed);

	/* Create a sketch memory context as a child of the current context, so it
	 * gets cleaned automatically in case of a ereport(ERROR) exit.
	 */
	private.sketch_context = AllocSetContextCreate(CurrentMemoryContext,
													"SAIO",
													ALLOCSET_DEFAULT_MINSIZE,
													ALLOCSET_DEFAULT_INITSIZE,
													ALLOCSET_DEFAULT_MAXSIZE);

	/* Set the number of loops before considering equilibrium */
	private.equilibrium_loops = levels_needed * saio_equilibrium_factor;
	/* Set the initial temperature */
	private.temperature = (double) private.previous_cost;
	private.temperature *= saio_initial_temperature_factor;
	/* Initialize the elapsed loops and failed moves counters */
	private.elapsed_loops = 0;
	private.failed_moves = 0;

	/*
	 * Build a query tree from the initial relations. This should a tree that
	 * represents any valid join order for the given set of rels.
	 * Do it in a sketch context to avoid polluting root->join_rel_list and
	 * root->join_rel_hash and to be able to free the memory taken by
	 * constructing paths after determining an initial join order.
	 */
	context_enter(root);

	tree = make_query_tree(root, initial_rels);
	/* Set the initial tree cost */
	private.previous_cost = SAIO_COST(tree->rel);

	/*
	 * Copy the tree structure to the correct memory context. The rest of the
	 * memory allocated in make_query_tree() will get freed in context_exit().
	 */
	MemoryContextSwitchTo(private.old_context);
	tree = copy_tree_structure(tree);
	MemoryContextSwitchTo(private.sketch_context);

	context_exit(root);

	/*
	 * Get the list of all trees to then pick randomly from them when doing SA
	 * algorithm moves.
	 */
	all_trees = get_all_nodes(tree);

	do {

		do {
			if (saio_move(root, tree, all_trees))
				private.failed_moves = 0;
			else
				private.failed_moves++;
		} while (!equilibrium(root));

		reduce_temperature(root);

	} while (!frozen(root));


	/* Rebuild the final rel in the correct memory context */
	recalculate_tree(root, tree);

	list_free(all_trees);

	return tree->rel;
}
