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

#ifndef SAIO_ALGORITHMS_H
#define SAIO_ALGORITHMS_H

#include "postgres.h"

#include "nodes/relation.h"
#include "nodes/pg_list.h"

int saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees);
int saio_pivot_move(PlannerInfo *root, QueryTree *tree, List *all_trees);
int saio_recalc_move(PlannerInfo *root, QueryTree *tree, List *all_trees);


#endif	/* SAIO_ALGORITHMS_H */
