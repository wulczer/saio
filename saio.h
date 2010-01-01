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

/*
 * A context under which a join order is computed. It will keep the state of
 * variables that get modified during planning and have to be reset to their
 * original state when the planning ends;
 *
 * It also holds a sketch memory context that will hold all memory allocations
 * done during considering a given join order. Since we are going to consider
 * lots of them, we need to free the memory after each try.
 *
 * See geqo_eval() for similar code and explanations.
 */
typedef struct SaioContext {
	MemoryContext	old_context;
	MemoryContext	sketch_context;
	int				savelength;
	struct HTAB		*savehash;
} SaioContext;


typedef struct QueryTree {
	RelOptInfo			*rel;
	struct QueryTree	*left;
	struct QueryTree	*right;
	struct QueryTree	*parent;
} QueryTree;


typedef struct SaioPrivateData {
	List					*costs;
	SaioJoinOrderContext	ctx;
} SaioPrivateData;


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif   /* SAIO_H */
