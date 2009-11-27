#ifndef SAIO_DEBUG_H
#define SAIO_DEBUG_H

#include "saio.h"
#include "optimizer/paths.h"

void debug_print_rel(PlannerInfo *root, RelOptInfo *rel);

void debug_dump_query_tree(PlannerInfo *root, QueryTree *tree);

#endif /* SAIO_DEBUG_H */
