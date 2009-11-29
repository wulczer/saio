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

#include "optimizer/paths.h"
#include "optimizer/pathnode.h"

#include "saio_debug.h"
#include "saio.h"



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

			new_tree = (QueryTree *) palloc(sizeof(QueryTree));
			set_cheapest(joinrel);

			new_tree->rel = joinrel;
			new_tree->left = old_tree;
			new_tree->right = tree;
			new_tree->parent = NULL;
			old_tree->parent = new_tree;
			tree->parent = new_tree;

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
		tree = (QueryTree *) palloc(sizeof(QueryTree));
		tree->rel = rel;
		tree->left = NULL;
		tree->right = NULL;
		tree->parent = NULL;

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
get_parents_rec(QueryTree *tree, List *res)
{
	Assert(tree != NULL);

	while (tree != NULL)
	{
		res = lcons(tree, res);
		tree = tree->parent;
	}
	return res;
}

static List *
get_parents(QueryTree *tree)
{
	return get_parents_rec(tree, NIL);
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
		return list_make1(parent->right);

	return NIL; /* keep compiler quiet */
}

static QueryTree *
saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	QueryTree	*tree1, *tree2;

	if (list_length(all_trees) == 1)
		return tree;

	debug_print_query_tree_list("All trees: ", all_trees);

	choices = list_copy(all_trees);
	choices = list_delete_ptr(choices, llast(choices));

	debug_print_query_tree_list("Choices for first node: ", choices);

	tree1 = list_nth(choices, random() % list_length(choices));

	debug_print_query_tree_list("First node: ", list_make1(tree1));

	tmp = get_tree_subtree(tree1);
	tmp = list_concat_unique_ptr(get_parents(tree1), tmp);
	tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
	choices = list_difference_ptr(choices, tmp);

	debug_print_query_tree_list("Choices for second node: ", choices);

	if (choices == NIL)
		return tree;

	tree2 = list_nth(choices, random() % list_length(choices));

	debug_print_query_tree_list("Second node: ", list_make1(tree2));
	debug_dump_query_tree(root, tree, tree1, tree2, "/tmp/move.dot");

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

	debug_dump_query_tree(root, tree, NULL, NULL, "/tmp/transformed.dot");

	debug_verify_query_tree(tree);

	res = tree->rel;

	return res;
}
