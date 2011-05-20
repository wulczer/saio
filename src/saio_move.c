/*------------------------------------------------------------------------
 *
 * saio_main.c
 *	  setup for Simulated Annealing query optimization
 *
 * Copyright (c) 2009, PostgreSQL Global Development Group
 *
 * $PostgreSQL$
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include <math.h>

#include "nodes/pg_list.h"
#include "nodes/relation.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"

#include "saio.h"
#include "saio_util.h"
#include "saio_trees.h"
#include "saio_debug.h"

static saio_result
saio_move_step(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	QueryTree	*tree1, *tree2;
	Cost		new_cost;
	bool		ok;
	SaioPrivateData	*private;

	/* if less than four trees to choose from, return immediately */
	if (list_length(all_trees) < 4)
		return SAIO_MOVE_IMPOSSIBLE;

	private = (SaioPrivateData *) root->join_search_private;

	context_enter(root);

	choices = list_copy(all_trees);
	choices = list_delete_ptr(choices, llast(choices));

	tree1 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	tmp = get_all_nodes(tree1);
	tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
	tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
	choices = list_difference_ptr(choices, tmp);

	snprintf(path, 256, "/tmp/saio-move-%04d-try.dot", private->loop_no);
	dump_query_tree_list(root, tree, tree1, NULL, choices, false, path);

	if (choices == NIL)
	{
		context_exit(root);
		return SAIO_MOVE_IMPOSSIBLE;
	}

	tree2 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	swap_subtrees(tree1, tree2);

	ok = recalculate_tree(root, tree);

	if (!ok)
	{
		swap_subtrees(tree1, tree2);
		context_exit(root);
		return SAIO_MOVE_FAILED;
	}

	new_cost = SAIO_COST(tree->rel);

	ok = acceptable(root, new_cost);

	if (!ok)
	{
		snprintf(path, 256, "/tmp/saio-move-%04d-failed.dot", private->loop_no);
		dump_query_tree(root, tree, tree1, tree2, true, path);
		swap_subtrees(tree1, tree2);
		context_exit(root);
		return SAIO_MOVE_DISCARDED;
	}

	snprintf(path, 256, "/tmp/saio-move-%04d-successful.dot", private->loop_no);
	dump_query_tree(root, tree, tree1, tree2, true, path);

	keep_minimum_state(root, tree, new_cost);
	private->previous_cost = new_cost;

	context_exit(root);

	return SAIO_MOVE_OK;
}

SaioAlgorithm saio_move = {
	.step = saio_move_step,
	.initialize = NULL,
	.finalize = NULL
};
