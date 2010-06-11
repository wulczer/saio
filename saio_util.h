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

#ifndef SAIO_UTIL_H
#define SAIO_UTIL_H

#include "postgres.h"

#define saio_randint(root, upper, lower) \
	( (int) floor( saio_rand(root)*(((upper)-(lower))+0.999999) ) + (lower) )

bool desirable_join(PlannerInfo *root,
					RelOptInfo *outer_rel, RelOptInfo *inner_rel);

void initialize_random_state(PlannerInfo *root, double seed);

double saio_rand(PlannerInfo *root);

#endif
