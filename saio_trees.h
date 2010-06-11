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

#ifndef SAIO_UTILS_H
#define SAIO_UTILS_H

#include "postgres.h"

QueryTree *make_query_tree(PlannerInfo *root, List *initial_rels);
QueryTree *copy_tree_structure(QueryTree *tree);
void keep_minimum_state(PlannerInfo *root, QueryTree *tree, Cost new_cost);

List *get_all_nodes(QueryTree *tree);
List *get_parents(QueryTree *tree, bool reverse);
List *get_siblings(QueryTree *tree);
List *filter_leaves(List *trees);
void swap_subtrees(QueryTree *tree1, QueryTree *tree2);


bool recalculate_tree(PlannerInfo *root, QueryTree *tree);
bool recalculate_tree_cutoff(PlannerInfo *root, QueryTree *tree,
							 QueryTree *cutoff);
bool recalculate_tree_cutoff_ctx(PlannerInfo *root, QueryTree *tree,
								 QueryTree *cutoff, bool own_ctx);

#endif	/* SAIO_UTILS_H */
