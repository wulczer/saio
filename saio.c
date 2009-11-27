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
	ListCell   *prev;
	ListCell   *lc;

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

			result = list_delete_cell(result, lc, prev);

			return merge_trees(root, result, new_tree);
		}
		prev = lc;
	}

	return lappend(result, tree);
}

static QueryTree *
make_query_tree(PlannerInfo *root, List *initial_rels)
{
	List		*result;
	ListCell	*lc;

	Assert(list_length(initial_rels) > 0);

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

		result = merge_trees(root, result, tree);
	}

	/* Did we succeed in forming a single join relation? */
	if (list_length(result) != 1)
		elog(ERROR, "failed to join all relations together");

	return (QueryTree *) linitial(result);
}

RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	/* ListCell	*l; */
	RelOptInfo	*res;
	QueryTree	*tree;

	/* foreach(l, initial_rels) */
	/* { */
	/* 	RelOptInfo *rel = lfirst(l); */

	/* 	debug_print_rel(root, rel); */
	/* } */

	tree = make_query_tree(root, initial_rels);

	debug_dump_query_tree(root, tree);

	res = standard_join_search(root, levels_needed, initial_rels);

	/* debug_print_rel(root, res); */
	return res;
}
