#ifndef SAIO_DEBUG_H
#define SAIO_DEBUG_H

#include "saio.h"
#include "optimizer/paths.h"

void debug_print_rel(PlannerInfo *root, RelOptInfo *rel);

void debug_dump_query_tree(PlannerInfo *root, QueryTree *tree, QueryTree
						   *selected1, QueryTree *selected2, char *path);

void debug_dump_query_tree_selected(PlannerInfo *root, QueryTree *tree,
									QueryTree *selected1, QueryTree *selected2,
									char *path);

void debug_verify_query_tree(QueryTree *tree);

void debug_print_query_tree_list(char *intro, List *trees);

#endif /* SAIO_DEBUG_H */
