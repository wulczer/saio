/*------------------------------------------------------------------------
 *
 * saio.c
 *	  solution to the query optimization problem
 *	  by means of Simulated Annealing (SA)
 *
 * Copyright (c) 2009, PostgreSQL Global Development Group
 *
 * $PostgreSQL$
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <math.h>

#include "utils/memutils.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"

#include "saio.h"
#include "saio_util.h"
#include "saio_trees.h"
#include "saio_debug.h"

extern SaioAlgorithm algorithm;

/*
 * Save the current state of the variables that get modified during
 * make_join_rel(). Enter a temporary memory context that will get reset when
 * we leave the context. See geqo_eval() for similar code and explanations.
 */
void
context_enter(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* join_rel_list and join_rel_hash get added to in make_join_rel() */
	private->savelength = list_length(root->join_rel_list);
	private->savehash = root->join_rel_hash;
	/* if a hash has already been built, we need to get rid of it */
	root->join_rel_hash = NULL;

	/* switch to the sketch context */
	private->old_context = MemoryContextSwitchTo(private->sketch_context);
}


/*
 * Restore the state, reset the sketch memory context.
 */
void
context_exit(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* restore join_rel_list and join_rel_hash */
	root->join_rel_list = list_truncate(root->join_rel_list,
										private->savelength);
	root->join_rel_hash = private->savehash;

	/* switch back to the old context */
	MemoryContextSwitchTo(private->old_context);

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(private->sketch_context);
}


void
context_enter_mem(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* switch to the sketch context */
	private->old_context = MemoryContextSwitchTo(private->sketch_context);
}


void
context_exit_mem(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;
	MemoryContextSwitchTo(private->old_context);

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(private->sketch_context);
}


bool
acceptable(PlannerInfo *root, Cost new_cost)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	return compare_costs(root, private->previous_cost,
						 new_cost, private->temperature);
}


bool
compare_costs(PlannerInfo *root, Cost previous_cost,
			  Cost new_cost, double temperature)
{
	/* downhill moves are always acceptable */
	if (new_cost < previous_cost)
		return true;

	/*
	 * If temperature < 1, moves that do not change state are considered
	 * unacceptable.
	 *
	 * FIXME: this is to avoid endless loop with the same temperature and state
	 * that is not changing, figure out *why* that happens and prevent it
	 */
	if ((temperature < 1) && (previous_cost == new_cost))
		return false;

	/*
	 * Uphill moves are acceptable with probability
	 *  exp((old - new) / temperature)
	 */
	return (saio_rand(root) < exp(((double) (previous_cost - new_cost)) / temperature));
}


static bool
equilibrium(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->elapsed_loops++;

	if (private->elapsed_loops >= private->equilibrium_loops)
	{
		/* reset the elapsed loops count */
		private->elapsed_loops = 0;
		/* equilibrium reached */
		return true;
	}

	return false;
}


static void
reduce_temperature(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->temperature *= saio_temperature_reduction_factor;
}


static bool
frozen(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* can only be frozen when temperature < 1 */
	if (private->temperature > 1)
		return false;

	/* check the number of consecutive failed moves */
	return private->failed_moves >= saio_moves_before_frozen;
}


RelOptInfo *
saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	QueryTree		*tree;
	List			*all_trees;
	RelOptInfo		*res;
	SaioPrivateData	private;
	bool			ok;


	/* Initialize private data */
	root->join_search_private = (void *) &private;

	/* Initialize the random state */
	initialize_random_state(root, saio_seed);

	/*
	 * Create a sketch memory context as a child of the current context, so it
	 * gets cleaned automatically in case of a ereport(ERROR) exit.
	 */
	private.sketch_context = AllocSetContextCreate(CurrentMemoryContext,
												   "SAIO",
												   ALLOCSET_DEFAULT_MINSIZE,
												   ALLOCSET_DEFAULT_INITSIZE,
												   ALLOCSET_DEFAULT_MAXSIZE);

	/* Create a context for keeping the minimum state */
	private.min_context = AllocSetContextCreate(CurrentMemoryContext,
												"SAIO min",
												ALLOCSET_DEFAULT_MINSIZE,
												ALLOCSET_DEFAULT_INITSIZE,
												ALLOCSET_DEFAULT_MAXSIZE);
	/*
	 * Build a query tree from the initial relations. This should a tree that
	 * represents any valid join order for the given set of rels.
	 * Do it in a sketch context to avoid polluting root->join_rel_list and
	 * root->join_rel_hash and to be able to free the memory taken by
	 * constructing paths after determining an initial join order.
	 */
	context_enter(root);

	tree = make_query_tree(root, initial_rels);
	/* Set the initial tree cost */
	private.previous_cost = SAIO_COST(tree->rel);

	/*
	 * Copy the tree structure to the correct memory context. The rest of the
	 * memory allocated in make_query_tree() will get freed in context_exit().
	 */
	MemoryContextSwitchTo(private.old_context);
	tree = copy_tree_structure(tree);
	MemoryContextSwitchTo(private.sketch_context);

	context_exit(root);

	/* Set the number of loops before considering equilibrium */
	private.equilibrium_loops = levels_needed * saio_equilibrium_factor;
	/* Set the initial temperature */
	private.temperature = (double) private.previous_cost;
	private.temperature *= saio_initial_temperature_factor;
	/* Initialize the elapsed loops and failed moves counters */
	private.elapsed_loops = 0;
	private.failed_moves = 0;
	/* Initialize the minimal state */
	private.min_tree = NULL;

	/* init debugging */
	private.steps = NIL;
	private.joinrels_built = 0;
	private.loop_no = 0;

	/* initialize the algorithm */
	if (algorithm.initialize != NULL)
		algorithm.initialize(root, tree);

	/*
	 * Get the list of all trees to then pick randomly from them when doing SA
	 * algorithm moves.
	 */
	all_trees = get_all_nodes(tree);

	do {

		do {
			saio_result	move_result = SAIO_MOVE_OK;
#ifdef SAIO_STATS
			/* save values for debugging */
			SaioStep	*step = palloc(sizeof(SaioStep));
#endif
			move_result = algorithm.step(root, tree, all_trees);

			if (move_result == SAIO_MOVE_OK)
			{
#ifdef SAIO_STATS
				step->move_result = SAIO_MOVE_OK;
#endif
				private.loop_no++;
				private.failed_moves = 0;
			}
			else
			{
#ifdef SAIO_STATS
				step->move_result = move_result;
#endif
				private.loop_no++;
				private.failed_moves++;
			}
#ifdef SAIO_STATS
			step->cost = private.previous_cost;
			step->temperature = private.temperature;
			step->joinrels_built = private.joinrels_built;
			private.steps = lappend(private.steps, step);
#endif
			private.joinrels_built = 0;
			elog(DEBUG1, "[%04d] at the end of the loop min tree is %p with cost %10.4f\n",
				 private.loop_no, private.min_tree,
				 private.min_tree == NULL ? 0 : private.min_cost);

		} while (!equilibrium(root));

		reduce_temperature(root);

	} while (!frozen(root));

#ifdef SAIO_STATS
	/* dump debugging values, free memory */
	dump_debugging(&private);
	list_free_deep(private.steps);
#endif

	elog(DEBUG1, "[%04d] at the end of the algorithm min tree is %p with cost %10.4f\n",
		 private.loop_no, private.min_tree,
		 private.min_tree == NULL ? 0 : private.min_cost);

	/* if there is a global minimum, pick it */
	if (private.min_tree != NULL)
	{
		tree = private.min_tree;
		elog(DEBUG1, "The cheapest tree is %10.4f\n", private.min_cost);
	}

	/* Finalize the algorithm */
	if (algorithm.finalize != NULL)
		algorithm.finalize(root, tree);

	/* Rebuild the final rel in the correct memory context */
	ok = recalculate_tree(root, tree);
	Assert(ok);
	snprintf(path, 256, "/tmp/saio-final.dot");
	dump_query_tree(root, tree, NULL, NULL, true, path);
	res = tree->rel;

	/* Clean up */
	list_free(all_trees);
	MemoryContextDelete(private.sketch_context);
	MemoryContextDelete(private.min_context);
	root->join_search_private = NULL;
	fflush(stdout);

	return res;
}
