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


typedef struct SAIOPrivate {
	List	*costs;
} SAIOPrivate;


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif   /* SAIO_H */
