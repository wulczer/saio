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
		QueryTree	*old_tree = (QueryTree *) lfirst(lc);
		RelOptInfo	*joinrel;

		joinrel = make_join_rel(root, old_tree->rel, tree->rel);

		if (joinrel)
		{
			QueryTree	*new_tree;

			new_tree = (QueryTree *) palloc0(sizeof(QueryTree));
			set_cheapest(joinrel);

			Assert(old_tree != NULL);
			Assert(tree != NULL);

			new_tree->rel = joinrel;
			new_tree->left = old_tree;
			new_tree->right = tree;
			new_tree->parent = NULL;
			new_tree->tmp = NULL;

			old_tree->parent = new_tree;
			tree->parent = new_tree;

			debug_verify_query_tree(new_tree);

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
		tree->parent = NULL;
		tree->left = NULL;
		tree->right = NULL;
		tree->tmp = NULL;
		tree->rel = rel;

		debug_verify_query_tree(tree);

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

	while (tree != NULL)
	{
		if (reverse)
			res = lappend(res, tree);
		else
			res = lcons(tree, res);
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

	return NIL; /* keep compiler quiet */
}

static bool
recalculate_trees(PlannerInfo *root, List *trees)
{
	ListCell	*lc;
	RelOptInfo	*joinrel;
	RelOptInfo	*left, *right;
	bool		ok;

	ok = true;

	debug_print_query_tree_list("Recalculating trees: ", trees);

	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		Assert(tree != NULL);

		printf("Recalculating (");
		fprint_relids(stdout, tree->rel->relids);
		printf("), ok = %d\n", ok);

		if (!ok)
			continue;

		if (tree->left == NULL)
		{
			Assert(tree->right == NULL);
			tree->tmp = tree->rel;
			continue;
		}

		Assert(tree->right != NULL);

		left = tree->left->tmp ? tree->left->tmp : tree->left->rel;
		right = tree->right->tmp ? tree->right->tmp : tree->right->rel;

		printf("Recalculating: left = (");
		fprint_relids(stdout, left->relids);
		printf(") right = (");
		fprint_relids(stdout, right->relids);
		printf(")\n");

		joinrel = make_join_rel(root, left, right);

		if (joinrel)
		{
			set_cheapest(joinrel);
			tree->tmp = joinrel;
		}
		else
		{
			ok = false;
		}
	}

	return ok;
}

static void
cleanup_tmp_path(List *trees, bool keep)
{
	ListCell	*lc;

	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		if (keep)
		{
			printf("Keeping the state of (");
			fprint_relids(stdout, tree->rel->relids);
			printf(")\n");
			Assert(tree->tmp != NULL);
			tree->rel = tree->tmp;
		}
		tree->tmp = NULL;
	}
}

static bool
compare_tree(PlannerInfo *root, QueryTree *tree)
{
	Assert(tree != NULL);
	Assert(tree->tmp != NULL);
	return (tree->tmp->cheapest_total_path->total_cost <
			tree->rel->cheapest_total_path->total_cost);
}

static bool
do_move(PlannerInfo *root, QueryTree *tree,
		QueryTree *tree1, QueryTree *tree2)
{
	QueryTree	*parent1, *parent2;
	List		*t1, *t2, *common;
	bool 		ok;

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

	debug_dump_query_tree(root, tree, tree1, tree2, "/tmp/switched.dot");

	t1 = get_parents(tree1, true);
	t2 = get_parents(tree2, true);
	common = list_intersection_ptr(t1, t2);

	debug_print_query_tree_list("t1: ", t1);
	debug_print_query_tree_list("t2: ", t2);
	debug_print_query_tree_list("common: ", common);

	t1 = list_difference_ptr(t1, common);

	debug_print_query_tree_list("t1 - common: ", t1);

	ok = recalculate_trees(root, t1);
	if (ok)
		ok = recalculate_trees(root, t2);

	if (ok)
		ok = compare_tree(root, tree);

	printf("Keeping the state: %d\n", ok);
	cleanup_tmp_path(t1, ok);
	cleanup_tmp_path(t2, ok);

	if (!ok)
	{
		tree1->parent = parent1;
		tree2->parent = parent2;

		if (parent1->left == tree2)
			parent1->left = tree1;
		else
			parent1->right = tree1;

		if (parent2->left == tree1)
			parent2->left = tree2;
		else
			parent2->right = tree2;
	}

	return ok;
}


#define CUTOFF 30

static QueryTree *
saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	QueryTree	*tree1, *tree2;
	int			loops;
	bool		ok;

	if (list_length(all_trees) == 1)
		return tree;

	loops = 0;

	debug_print_query_tree_list("All trees: ", all_trees);

	while (loops++ < CUTOFF)
	{
		char dump_before[NAMEDATALEN];
		char dump_after[NAMEDATALEN];

		snprintf(dump_before, NAMEDATALEN, "/tmp/before-move-%d.dot", loops);
		snprintf(dump_after, NAMEDATALEN, "/tmp/after-move-%d.dot", loops);

		choices = list_copy(all_trees);
		choices = list_delete_ptr(choices, llast(choices));

		debug_print_query_tree_list("Choices for first node: ", choices);

		tree1 = list_nth(choices, random() % list_length(choices));

		debug_print_query_tree_list("First node: ", list_make1(tree1));

		tmp = get_tree_subtree(tree1);
		tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
		tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
		choices = list_difference_ptr(choices, tmp);

		debug_print_query_tree_list("Choices for second node: ", choices);

		if (choices == NIL)
			return tree;

		tree2 = list_nth(choices, random() % list_length(choices));

		debug_print_query_tree_list("Second node: ", list_make1(tree2));
		debug_dump_query_tree(root, tree, tree1, tree2, dump_before);

		printf("Move starting\n");
		ok = do_move(root, tree, tree1, tree2);
		printf("Move finished\n");

		debug_dump_query_tree(root, tree, tree1, tree2, dump_after);
		printf("Dump finished\n");
	}

	return tree;
}

RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	RelOptInfo	*res;
	QueryTree	*tree;
	List		*all_trees;

	tree = make_query_tree(root, initial_rels);

	all_trees = get_tree_subtree(tree);

	debug_dump_query_tree(root, tree, NULL, NULL, "/tmp/original.dot");

	tree = saio_move(root, tree, all_trees);

	debug_dump_query_tree(root, tree, NULL, NULL, "/tmp/final.dot");

	debug_verify_query_tree(tree);

	res = tree->rel;

	return res;
}
