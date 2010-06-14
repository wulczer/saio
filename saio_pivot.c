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

static void
execute_pivot(QueryTree *pivot_root)
{
	QueryTree	*a, *b, *c;

	a = pivot_root->left;
	b = pivot_root->right;
	c = OTHER_CHILD(pivot_root->parent, pivot_root);

	swap_subtrees(a, c);
}


static saio_result
saio_pivot_step(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices;
	QueryTree	*pivot_root;
	SaioPrivateData	*private;

	/* if less than four trees to choose from, return immediately */
	if (list_length(all_trees) < 4)
		return SAIO_MOVE_IMPOSSIBLE;

	private = (SaioPrivateData *) root->join_search_private;

	/* get all trees */
	choices = list_copy(all_trees);
	/* remove root */
	choices = list_delete_ptr(choices, llast(choices));
	/* remove leaves */
	choices = filter_leaves(choices);

	while (choices != NIL)
	{
		bool	ok;
		Cost	new_cost;

		/* pick pivot root */
		pivot_root = list_nth(choices, saio_randint(
								  root, 0, list_length(choices) - 1));
		choices = list_delete_ptr(choices, pivot_root);

		Assert(pivot_root->left != NULL);
		Assert(pivot_root->right != NULL);
		Assert(pivot_root->parent != NULL);

		snprintf(path, 256, "/tmp/saio-pivot-%04d-try.dot", private->loop_no);
		dump_query_tree_list(root, tree, pivot_root, NULL, choices, false, path);

		execute_pivot(pivot_root);

		context_enter(root);

		ok = recalculate_tree(root, pivot_root->parent);

		if (!ok)
		{
			execute_pivot(pivot_root);
			context_exit(root);
			continue;
		}

		ok = recalculate_tree_cutoff(root, tree, pivot_root->parent);
		Assert(ok);

		new_cost = SAIO_COST(tree->rel);

		ok = acceptable(root, new_cost);

		if (!ok)
		{
			snprintf(path, 256, "/tmp/saio-pivot-%04d-failed.dot", private->loop_no);
			dump_query_tree_list(root, tree, pivot_root, NULL, choices, true, path);
			execute_pivot(pivot_root);
			context_exit(root);
			continue;
		}

		snprintf(path, 256, "/tmp/saio-pivot-%04d-successful.dot", private->loop_no);
		dump_query_tree_list(root, tree, pivot_root, NULL, choices, true, path);
		keep_minimum_state(root, tree, new_cost);
		private->previous_cost = new_cost;

		context_exit(root);
		list_free(choices);
		return SAIO_MOVE_OK;
	}

	list_free(choices);
	return SAIO_MOVE_FAILED;
}


#ifdef NOT_USED
static bool
pivot_is_possible(PlannerInfo *root, QueryTree *pivot_root)
{
	QueryTree	*a, *b, *c;
	RelOptInfo	*r1, *r2;

	a = pivot_root->left;
	b = pivot_root->right;
	c = OTHER_CHILD(pivot_root->parent, pivot_root);

	r1 = make_join_rel(root, b->rel, c->rel);
	if (r1 == NULL)
		return false;

	set_cheapest(r1);

	r2 = make_join_rel(root, a->rel, r1);
	if (r2 == NULL)
		return false;

	set_cheapest(r2);

	return true;
}
#endif

SaioAlgorithm saio_pivot = {
	.step = saio_pivot_step,
	.initialize = NULL,
	.finalize = NULL
};
