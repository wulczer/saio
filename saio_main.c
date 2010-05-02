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

#include <limits.h>

#ifdef SAIO_DEBUG
#include <printf.h>
#include "saio_debug.h"
#endif

#include "utils/guc.h"
#include "optimizer/paths.h"
#include "optimizer/geqo.h"

#include "saio.h"

PG_MODULE_MAGIC;

static const struct config_enum_entry algorithm_options[] = {
    {"move", SAIO_ALGORITHM_MOVE, false},
    {"pivot", SAIO_ALGORITHM_PIVOT, false},
    {"recalc", SAIO_ALGORITHM_RECALC, false},
    {NULL, 0, false}
};

/* GUC variables */
bool	enable_saio = false;
double	saio_seed = 0.0;
int		saio_equilibrium_factor = 16;
double	saio_initial_temperature_factor = 2.0;
double	saio_temperature_reduction_factor = 0.9;
int		saio_moves_before_frozen = 4;

int saio_move_algorithm = SAIO_ALGORITHM_RECALC;

/* Saved hook value in case of unload */
static join_search_hook_type prev_join_search_hook = NULL;


static RelOptInfo *
saio_main(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	if (enable_saio)
		return saio(root, levels_needed, initial_rels);
	else if (enable_geqo && levels_needed >= geqo_threshold)
		return geqo(root, levels_needed, initial_rels);
	else
		return standard_join_search(root, levels_needed, initial_rels);
}

/* Module load */
void
_PG_init(void)
{
	 /* Define custom GUC */
	DefineCustomBoolVariable("saio", "Use SA for query planning.", NULL,
							 &enable_saio, true,
							 PGC_USERSET,
							 0, NULL, NULL);

	DefineCustomRealVariable("saio_seed",
							 "SA random seed.", NULL,
							 &saio_seed, 0.0, 0.0, 1.0,
							 PGC_USERSET,
							 0, NULL, NULL);

	DefineCustomIntVariable("saio_equilibrium_factor",
							"SA scaling factor for reaching equilibirum.", NULL,
							&saio_equilibrium_factor, 16, 1, INT_MAX,
							PGC_USERSET,
							0, NULL, NULL);

	DefineCustomRealVariable("saio_initial_temperature_factor",
							 "SA scaling factor for initial temperature.", NULL,
							 &saio_initial_temperature_factor, 2.0, 0.0, 10.0,
							 PGC_USERSET,
							 0, NULL, NULL);

	DefineCustomRealVariable("saio_temperature_reduction_factor",
							 "SA temperature reduction factor.", NULL,
							 &saio_temperature_reduction_factor, 0.9, 0.0, 1.0,
							 PGC_USERSET,
							 0, NULL, NULL);

	DefineCustomIntVariable("saio_moves_before_frozen",
							"SA moves before considering system frozen.", NULL,
							&saio_moves_before_frozen, 4, 1, INT_MAX,
							PGC_USERSET,
							0, NULL, NULL);

	DefineCustomEnumVariable("saio_algorithm", "Algorithm to use for moves.",
							 NULL,
							 &saio_move_algorithm, SAIO_ALGORITHM_MOVE,
							 algorithm_options, PGC_USERSET, 0, NULL, NULL);
	/* Install hook */
	prev_join_search_hook = join_search_hook;
	join_search_hook = saio_main;

#ifdef SAIO_DEBUG
	register_printf_specifier('T', print_tree_node, print_tree_node_arginfo);
#endif
}

/* Module unload */
void
_PG_fini(void)
{
	/* Uninstall hook */
	join_search_hook = prev_join_search_hook;
}
