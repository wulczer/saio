#ifndef SAIO_DEBUG_H
#define SAIO_DEBUG_H

#include "saio.h"
#include "optimizer/paths.h"

void fprintf_relids(FILE *f, Relids relids);

void dump_query_tree(PlannerInfo *root, QueryTree *tree, QueryTree *selected1,
					 QueryTree *selected2, char *path);

void verify_query_tree(QueryTree *tree);

void print_query_tree_list(char *intro, List *trees);

#endif /* SAIO_DEBUG_H */
