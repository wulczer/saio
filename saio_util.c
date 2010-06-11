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

#include <time.h>

#include "optimizer/joininfo.h"
#include "optimizer/paths.h"

#include "saio.h"
#include "saio_util.h"

/* verbatim copy from geqo_eval.c */
bool
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


/* verbatim copy from geqo_random.c */
void
initialize_random_state(PlannerInfo *root, double seed)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	if (seed == 0)
		seed = (double) time(NULL);

	memset(private->random_state, 0, sizeof(private->random_state));
	memcpy(private->random_state,
		   &seed,
		   Min(sizeof(private->random_state), sizeof(seed)));
}


double
saio_rand(PlannerInfo *root)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	return erand48(private->random_state);
}
