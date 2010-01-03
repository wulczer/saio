/*
 * $PostgreSQL$
 *
 *
 * saio
 *
 * A Simulated Annealing algorithm for solving the query join order problem.
 *
 * Copyright (c) 2009, PostgreSQL Global Development Group
 *
 */

#ifndef SAIO_H
#define SAIO_H

#include "postgres.h"

#include "nodes/pg_list.h"
#include "nodes/bitmapset.h"
#include "optimizer/paths.h"

extern int saio_cutoff;


typedef struct QueryTree {
	RelOptInfo			*rel;
	struct QueryTree	*left;
	struct QueryTree	*right;
	struct QueryTree	*parent;
} QueryTree;


/*
 * Keep the the state of variables that get modified during planning and have
 * to be reset to their original state when the planning ends;
 *
 * Also use a sketch memory context that will hold all memory allocations done
 * during considering a given join order. Since we are going to consider lots
 * of them, we need to free the memory after each try.
 *
 * See geqo_eval() for similar code and explanations.
 */
typedef struct SaioPrivateData {
	Cost			previous_cost;		/* the cost of the previously join order */
	MemoryContext	old_context;		/* the saved memory context */
	MemoryContext	sketch_context;		/* the sketch memory context for
										 * allocations during joinrel creation */
	int				savelength;			/* length of planner's join_rel_list */
	struct HTAB		*savehash;			/* the planner's join_rel_hash */

	/* debugging only */
	int				loops;
} SaioPrivateData;


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif   /* SAIO_H */
