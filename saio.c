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

#include "saio_debug.h"
#include "saio.h"


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


static List *
merge_trees(PlannerInfo *root, List *result, QueryTree *tree)
{
	ListCell	*prev;
	ListCell	*lc;

	prev = NULL;
	foreach(lc, result)
	{
		QueryTree	*other_tree = (QueryTree *) lfirst(lc);
		RelOptInfo	*joinrel;

		joinrel = make_join_rel(root, other_tree->rel, tree->rel);

		if (joinrel)
		{
			QueryTree	*new_tree;

			new_tree = (QueryTree *) palloc0(sizeof(QueryTree));

			new_tree->rel = joinrel;

			new_tree->left = other_tree;
			other_tree->parent = new_tree;

			new_tree->right = tree;
			tree->parent = new_tree;

			set_cheapest(joinrel);

			result = list_delete_cell(result, lc, prev);

			return merge_trees(root, result, new_tree);
		}
		prev = lc;
	}

	return lcons(tree, result);
}

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

		result = merge_trees(root, result, tree);
	}

	/* Did we succeed in forming a single join relation? */
	if (list_length(result) != 1)
		elog(ERROR, "failed to join all relations together");

	return (QueryTree *) linitial(result);
}

static List *
get_tree_subtree_rec(QueryTree *tree, List *res)
{
	if (tree == NULL)
		return res;

	res = lcons(tree, res);
	get_tree_subtree_rec(tree->left, res);
	get_tree_subtree_rec(tree->right, res);
	return res;
}

static List *
get_tree_subtree(QueryTree *tree)
{
	return get_tree_subtree_rec(tree, NIL);
}

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

static List *
get_parents(QueryTree *tree, bool reverse)
{
	return get_parents_rec(tree, reverse, NIL);
}


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
	if (parent->right == tree)
		return list_make1(parent->left);

	Assert(false);
	return NIL; /* keep compiler quiet */
}


static bool
recalculate_tree(PlannerInfo *root, QueryTree *tree)
{
	RelOptInfo	*joinrel;
	bool		ok;

	Assert(tree != NULL);
	Assert(tree->left != NULL);
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

	printf("Recalculating: left = (");
	fprintf_relids(stdout, tree->left->rel->relids);
	printf(") right = (");
	fprintf_relids(stdout, tree->right->rel->relids);
	printf(")\n");

	joinrel = make_join_rel(root, tree->left->rel, tree->right->rel);
	if (joinrel == NULL)
		return false;

	set_cheapest(joinrel);
	printf("Built joinrel for (");
	fprintf_relids(stdout, joinrel->relids);
	printf(") with cheapest path is %.2f\n", joinrel->cheapest_total_path->total_cost);

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
	Assert(root->join_rel_level == NULL);

	root->join_rel_hash = NULL;

	ok = recalculate_tree(root, tree);

	root->join_rel_list = list_truncate(root->join_rel_list,
										savelength);
	root->join_rel_hash = savehash;

	dump_query_tree(root, tree, tree1, tree2, dump_middle);

	if (ok)
	{
		Assert(tree->rel != NULL);
		printf("Cost after move: %.2f\n", tree->rel->cheapest_total_path->total_cost);
		ok = (tree->rel->cheapest_total_path->total_cost < current_cost);
	}

	printf("Keeping the state: %d\n", ok);

	if (!ok)
		swap_subtrees(tree2, tree1);

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

		printf("Loop %d, current cost cost: %.2f\n", loops, current_cost);
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

		printf("Move starting\n");
		current_cost = do_move(root, tree, tree1, tree2, loops, current_cost);

		private = (SAIOPrivate *) root->join_search_private;
		ccost = (Cost *) palloc(sizeof(Cost));
		*ccost = current_cost;
		private->costs = lappend(private->costs, ccost);

		printf("Move finished\n");

		printf("Dump finished\n");
	}
}

RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	RelOptInfo	*res;
	QueryTree	*tree;
	List		*all_trees;
	int			savelength;
	struct HTAB *savehash;
	SAIOPrivate	private;

	root->join_search_private = (void *) &private;
	private.costs = NIL;

	savelength = list_length(root->join_rel_list);
	savehash = root->join_rel_hash;
	Assert(root->join_rel_level == NULL);

	tree = make_query_tree(root, initial_rels);

	Assert(tree->rel != NULL);

	root->join_rel_list = list_truncate(root->join_rel_list,
										savelength);
	root->join_rel_hash = savehash;

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
