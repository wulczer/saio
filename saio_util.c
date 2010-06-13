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


bool
join_can_be_legal(PlannerInfo *root, Relids relids1, Relids relids2,
				  Relids joinrelids)
{
	SpecialJoinInfo *match_sjinfo;
	bool		is_valid_inner;
	ListCell   *l;

	/*
	 * If we have any special joins, the proposed join might be illegal; and
	 * in any case we have to determine its join type.	Scan the join info
	 * list for conflicts.
	 */
	match_sjinfo = NULL;
	is_valid_inner = true;

	foreach(l, root->join_info_list)
	{
		SpecialJoinInfo *sjinfo = (SpecialJoinInfo *) lfirst(l);

		/*
		 * This special join is not relevant unless its RHS overlaps the
		 * proposed join.  (Check this first as a fast path for dismissing
		 * most irrelevant SJs quickly.)
		 */
		if (!bms_overlap(sjinfo->min_righthand, joinrelids))
			continue;

		/*
		 * Also, not relevant if proposed join is fully contained within RHS
		 * (ie, we're still building up the RHS).
		 */
		if (bms_is_subset(joinrelids, sjinfo->min_righthand))
			continue;

		/*
		 * Also, not relevant if SJ is already done within either input.
		 */
		if (bms_is_subset(sjinfo->min_lefthand, relids1) &&
			bms_is_subset(sjinfo->min_righthand, relids1))
			continue;
		if (bms_is_subset(sjinfo->min_lefthand, relids2) &&
			bms_is_subset(sjinfo->min_righthand, relids2))
			continue;

		/*
		 * If it's a semijoin and we already joined the RHS to any other rels
		 * within either input, then we must have unique-ified the RHS at that
		 * point (see below).  Therefore the semijoin is no longer relevant in
		 * this join path.
		 */
		if (sjinfo->jointype == JOIN_SEMI)
		{
			if (bms_is_subset(sjinfo->syn_righthand, relids1) &&
				!bms_equal(sjinfo->syn_righthand, relids1))
				continue;
			if (bms_is_subset(sjinfo->syn_righthand, relids2) &&
				!bms_equal(sjinfo->syn_righthand, relids2))
				continue;
		}

		/*
		 * If one input contains min_lefthand and the other contains
		 * min_righthand, then we can perform the SJ at this join.
		 *
		 * Barf if we get matches to more than one SJ (is that possible?)
		 */
		if (bms_is_subset(sjinfo->min_lefthand, relids1) &&
			bms_is_subset(sjinfo->min_righthand, relids2))
		{
			if (match_sjinfo)
				return false;	/* invalid join path */
			match_sjinfo = sjinfo;
		}
		else if (bms_is_subset(sjinfo->min_lefthand, relids2) &&
				 bms_is_subset(sjinfo->min_righthand, relids1))
		{
			if (match_sjinfo)
				return false;	/* invalid join path */
			match_sjinfo = sjinfo;
		}
		else
		{
			/*----------
			 * Otherwise, the proposed join overlaps the RHS but isn't
			 * a valid implementation of this SJ.  It might still be
			 * a legal join, however.  If both inputs overlap the RHS,
			 * assume that it's OK.  Since the inputs presumably got past
			 * this function's checks previously, they can't overlap the
			 * LHS and their violations of the RHS boundary must represent
			 * SJs that have been determined to commute with this one.
			 * We have to allow this to work correctly in cases like
			 *		(a LEFT JOIN (b JOIN (c LEFT JOIN d)))
			 * when the c/d join has been determined to commute with the join
			 * to a, and hence d is not part of min_righthand for the upper
			 * join.  It should be legal to join b to c/d but this will appear
			 * as a violation of the upper join's RHS.
			 * Furthermore, if one input overlaps the RHS and the other does
			 * not, we should still allow the join if it is a valid
			 * implementation of some other SJ.  We have to allow this to
			 * support the associative identity
			 *		(a LJ b on Pab) LJ c ON Pbc = a LJ (b LJ c ON Pbc) on Pab
			 * since joining B directly to C violates the lower SJ's RHS.
			 * We assume that make_outerjoininfo() set things up correctly
			 * so that we'll only match to some SJ if the join is valid.
			 * Set flag here to check at bottom of loop.
			 *----------
			 */
			if (sjinfo->jointype != JOIN_SEMI &&
				bms_overlap(relids1, sjinfo->min_righthand) &&
				bms_overlap(relids2, sjinfo->min_righthand))
			{
				/* seems OK */
				Assert(!bms_overlap(joinrelids, sjinfo->min_lefthand));
			}
			else
				is_valid_inner = false;
		}
	}

	/*
	 * Fail if violated some SJ's RHS and didn't match to another SJ. However,
	 * "matching" to a semijoin we are implementing by unique-ification
	 * doesn't count (think: it's really an inner join).
	 */
	if (!is_valid_inner && match_sjinfo == NULL)
		return false;			/* invalid join path */

	/* Otherwise, it's a valid join */
	return true;
}
