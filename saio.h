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
extern double	saio_seed;
extern int		saio_equilibrium_factor;
extern double	saio_initial_temperature_factor;
extern double	saio_temperature_reduction_factor;
extern int		saio_moves_before_frozen;
extern int		saio_move_algorithm;

/*
 * A tree that represents a join order.
 *
 * Each node has either one or no children. Leaves are initial relations, inner
 * nodes are intermediate joinrels and the root node is the final join
 * relation.
 */
typedef struct QueryTree {
	RelOptInfo			*rel;
	RelOptInfo			*tmp;
	struct QueryTree	*left;
	struct QueryTree	*right;
	struct QueryTree	*parent;
	MemoryContext		ctx;
	MemoryContext		tmpctx;
} QueryTree;


/*
 * Private data for the SAIO algorithm. Keeps the the state of variables that
 * get modified during planning and have to be reset to their original state
 * when the planning ends.
 *
 * Also keep a sketch memory context that will hold all memory allocations
 * done during considering a given join order. Since we are going to consider
 * lots of them, we need to free the memory after each try.
 */
typedef struct SaioPrivateData {
	MemoryContext	old_context;		/* the saved memory context */
	MemoryContext	sketch_context;		/* the sketch memory context */
	int				savelength;			/* length of planner's join_rel_list */
	struct HTAB		*savehash;			/* the planner's join_rel_hash */

	Cost			previous_cost;		/* the previously computed cost */
	Cost			min_cost;			/* globally cheapest cost */
	MemoryContext	min_context;		/* context for the cheapest tree */
	QueryTree		*min_tree;			/* globally cheapest tree */

	int				failed_moves;		/* moves that did not change the tree */
	int				equilibrium_loops;	/* loops before reaching equilibrium */
	int				elapsed_loops;		/* loops elapsed */
	double			temperature;		/* current system temperature */

	unsigned short	random_state[3];	/* state for erand48() */

	/* debugging aids */
	List			*steps;
	int				joinrels_built;
	int				loop_no;
} SaioPrivateData;


typedef enum saio_algorithm
{
	SAIO_ALGORITHM_MOVE = 0,
	SAIO_ALGORITHM_PIVOT,
	SAIO_ALGORITHM_RECALC,
} saio_algorithm;


enum
{
	SAIO_MOVE_OK = 0,
	SAIO_MOVE_FAILED,
	SAIO_MOVE_DISCARDED,
	SAIO_MOVE_IMPOSSIBLE
};


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif   /* SAIO_H */
