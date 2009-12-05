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

#include "utils/guc.h"
#include "optimizer/paths.h"
#include "optimizer/geqo.h"

#include "saio.h"

PG_MODULE_MAGIC;

/* GUC variables */
static bool enable_saio = false;

int	saio_cutoff = 1;

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
	DefineCustomBoolVariable("saio",
							 "Use SA for query planning.",
							 NULL,
							 &enable_saio,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL);

	DefineCustomIntVariable("saio_cutoff",
							"The cutoff for SAIO steps.",
							NULL,
							&saio_cutoff,
							1,
							0,
							INT_MAX,
							PGC_USERSET,
							0,
							NULL,
							NULL);

	/* Install hook */
	prev_join_search_hook = join_search_hook;
	join_search_hook = saio_main;
}

/* Module unload */
void
_PG_fini(void)
{
	/* Uninstall hook */
	join_search_hook = prev_join_search_hook;
}
