#ifndef SAIO_DEBUG_H
#define SAIO_DEBUG_H

#include "saio.h"
#include "optimizer/paths.h"

void fprintf_relids(FILE *f, Relids relids);

void dump_query_tree(PlannerInfo *root, QueryTree *tree, QueryTree *selected1,
					 QueryTree *selected2, char *path);

void dump_costs(PlannerInfo *root, char *path);

void verify_query_tree(QueryTree *tree, char *path);

void print_query_tree_list(char *intro, List *trees);

void trace_join(const char *path, RelOptInfo *r1, RelOptInfo *r2);

int debug_vprintf(FILE *f, const char *format, ...);
int debug_fprintf(FILE *f, const char *format, ...);
int debug_printf(const char *format, ...);

#endif /* SAIO_DEBUG_H */
