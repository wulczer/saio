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

#define SAIO_COST(rel) (rel)->cheapest_total_path->total_cost

#if PG_VERSION_NUM >= 90100
#define SAIO_GUC_HOOK_VALUES NULL, NULL, NULL
#else
#define SAIO_GUC_HOOK_VALUES NULL, NULL
#endif

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
	Cost				previous_cost;
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
} SaioPrivateData;


typedef enum saio_algorithm
{
	SAIO_ALGORITHM_MOVE = 0,
	SAIO_ALGORITHM_PIVOT,
	SAIO_ALGORITHM_RECALC,
} saio_algorithm;


typedef enum saio_result
{
	SAIO_MOVE_OK = 0,
	SAIO_MOVE_FAILED_FAST,
	SAIO_MOVE_FAILED,
	SAIO_MOVE_DISCARDED,
	SAIO_MOVE_IMPOSSIBLE
} saio_result;

typedef struct SaioAlgorithm {
	saio_result (*step) (PlannerInfo *root,
							  QueryTree *tree, List *all_trees);
	void (*initialize) (PlannerInfo *root, QueryTree *tree);
	void (*finalize) (PlannerInfo *root, QueryTree *tree);
} SaioAlgorithm;

void context_enter(PlannerInfo *root);
void context_exit(PlannerInfo *root);

void context_enter_mem(PlannerInfo *root);
void context_exit_mem(PlannerInfo *root);

bool acceptable(PlannerInfo *root, Cost new_cost);
bool compare_costs(PlannerInfo *root, Cost new_cost,
				   Cost previous_cost, double temperature);


RelOptInfo *saio(PlannerInfo *root, int levels_needed, List *initial_rels);

void _PG_init(void);
void _PG_fini(void);

#endif	/* SAIO_H */
