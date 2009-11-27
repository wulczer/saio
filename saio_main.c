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

#include "utils/guc.h"
#include "optimizer/paths.h"

#include "saio.h"

PG_MODULE_MAGIC;

/* GUC variables */
static bool enable_saio = false;

/* Saved hook value in case of unload */
static join_search_hook_type prev_join_search_hook = NULL;


static RelOptInfo *
saio_main(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	if (enable_saio)
		return saio(root, levels_needed, initial_rels);
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
