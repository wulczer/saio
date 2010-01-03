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


/* These get set by GUC */
extern int		saio_equilibrium_factor;
extern int		saio_initial_temperature_factor;
extern double	saio_temperature_reduction_factor;
extern int		saio_moves_before_frozen;


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
	MemoryContext	old_context;		/* the saved memory context */
	MemoryContext	sketch_context;		/* the sketch memory context */
	int				savelength;			/* length of planner's join_rel_list */
	struct HTAB		*savehash;			/* the planner's join_rel_hash */

	Cost			previous_cost;		/* the previously computere cost */
	List			*min_state_path;	/* list of node pairs to swap to swap
										 * to achieve the minimum cost state */

	int				failed_moves;		/* moves that did not change the tree */
	int				equilibrium_loops;	/* loops before reaching equilibrium */
	int				elapsed_loops;		/* loops elapsed */
	double			temperature;		/* current system temperature */
} SaioPrivateData;


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif   /* SAIO_H */
