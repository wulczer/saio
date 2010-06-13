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

#include <stdio.h>
#include <printf.h>

#include "optimizer/paths.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"

#include "saio.h"
#include "saio_debug.h"

#ifndef SAIO_DEBUG
#define printf(str, ...)
#define dump_query_tree_list2(root, tree, tree1, tree2, l1, l2, fake, path)
#define dump_query_tree_list(root, tree, tree1, tree2, l1, fake, path)
#define dump_query_tree(root, tree, tree1, tree2, fake, path);
#else

void
dump_debugging(SaioPrivateData *private)
{
	char path[1024];
	int suffix;
	int i = 0;

	for (suffix = 0; ; suffix++)
	{
		FILE		*f;
		ListCell	*lc;

		snprintf(path, 1024, "/tmp/saio.debug.%03d", suffix);
		if ((f = fopen(path, "r")) != NULL)
		{
			fclose(f);
			continue;
		}

		f = fopen(path, "w");
		Assert(f != NULL);

		foreach(lc, private->steps)
		{
			SaioStep	*step = (SaioStep *) lfirst(lc);

			fprintf(f, "%04d %10.4f  %10.4f  %1d  %6d\n",
					i++, step->temperature, step->cost,
					step->move_result, step->joinrels_built);
		}
		fclose(f);
		break;
	}
}


int
print_tree_node_arginfo(const struct printf_info *info, size_t n,
						int *argtypes, int *size)
{
	if (n > 0) {
		argtypes[0] = PA_POINTER;
		*size = sizeof(QueryTree *);
	}
	return 1;
}


int
print_tree_node(FILE *stream, const struct printf_info *info,
					const void *const *args)
{
	const QueryTree	*tree;
	char			*buffer;
	int				len;
	Relids			tmprelids;
	int				x;
	bool			first = true;
	int				bufsize;

	tree = *((QueryTree **) (args[0]));

	bufsize = 30;
	if (tree->rel != NULL)
		bufsize += bms_num_members(tree->rel->relids) * 4 + 2;
	if (tree->tmp != NULL)
		bufsize += bms_num_members(tree->tmp->relids) * 4 + 2;

	buffer = malloc(bufsize);
	if (!buffer)
		return -1;

	buffer[0] = '\0';
	len = sprintf(buffer, "[real: ");
	if (tree->rel != NULL)
	{
		tmprelids = bms_copy(tree->rel->relids);
		while ((x = bms_first_member(tmprelids)) >= 0)
		{
			len += sprintf(buffer + len, first ? "(" : " ");
			len += sprintf(buffer + len, "%d", x);
			first = false;
		}
		len += sprintf(buffer + len, ")");
		bms_free(tmprelids);
	}
	else
	{
		len += sprintf(buffer + len, "(nil)");
	}

	len += sprintf(buffer + len, "; tmp: ");
	if (tree->tmp != NULL)
	{
		first = true;
		tmprelids = bms_copy(tree->tmp->relids);
		while ((x = bms_first_member(tmprelids)) >= 0)
		{
			len += sprintf(buffer + len, first ? "(" : " ");
			len += sprintf(buffer + len, "%d", x);
			first = false;
		}
		len += sprintf(buffer + len, ")");
		bms_free(tmprelids);
	}
	else
	{
		len += sprintf(buffer + len, "(nil)");
	}
	sprintf(buffer + len, "]");

	len = fprintf(stream, "%*s", (info->left ? -info->width : info->width),
				  buffer);
	free(buffer);
	return len;
}


int
print_relids_arginfo(const struct printf_info *info, size_t n,
					 int *argtypes, int *size)
{
	if (n > 0) {
		argtypes[0] = PA_POINTER;
		*size = sizeof(Relids);
	}
	return 1;
}


int
print_relids(FILE *stream, const struct printf_info *info,
			 const void *const *args)
{
	Relids	relids;
	Relids	tmprelids;
	int		bufsize;
	char	*buffer;
	int		len;
	int		x;
	bool	first = true;

	relids = *((Relids *) (args[0]));

	bufsize = bms_num_members(relids) * 2;
	buffer = malloc(bufsize);
	if (!buffer)
		return -1;

	len = 0;
	tmprelids = bms_copy(relids);
	while ((x = bms_first_member(tmprelids)) >= 0)
	{
		if (!first)
			len += sprintf(buffer + len, " ");
		len += sprintf(buffer + len, "%d", x);
		first = false;
	}
	bms_free(tmprelids);

	len = fprintf(stream, "%*s", (info->left ? -info->width : info->width),
				  buffer);
	free(buffer);
	return len;
}


void
fprintf_relids(FILE *f, Relids relids)
{
	Relids		tmprelids;
	int			x;
	bool		first = true;

	/* for (x = 0; x < relids->nwords; x++) */
	/* { */
	/* 	if (!first) */
	/* 		fprintf(f, " "); */
	/* 	fprintf(f, "%u", relids->words[x]); */
	/* 	first = false; */
	/* } */

	tmprelids = bms_copy(relids);
	while ((x = bms_first_member(tmprelids)) >= 0)
	{
		if (!first)
			fprintf(f, " ");
		fprintf(f, "%d", x);
		first = false;
	}
	bms_free(tmprelids);
}


static void
dump_query_tree_node(QueryTree *tree,
					 QueryTree *selected1, QueryTree *selected2,
					 List *lselected1, List *lselected2,
					 Relids relids, bool costs,
					 Cost previous_cost, FILE *f)
{

	fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	fprintf(f, ")\"");
	if (tree == selected1)
		fprintf(f, " [color=red, fontcolor=red, ");
	else if (tree == selected2)
		fprintf(f, " [color=blue, fontcolor=blue, ");
	else if (list_member_ptr(lselected1, tree))
		fprintf(f, " [color=green, fontcolor=green, ");
	else if (list_member_ptr(lselected2, tree))
		fprintf(f, " [color=violet, fontcolor=violet, ");
	else
		fprintf(f, " [");

	fprintf(f, "label=<(");
	fprintf_relids(f, relids);
	fprintf(f, ")");
	if (costs)
		fprintf(f, "<br/><font point-size=\"7\">%.2f</font>",
					  tree->rel->cheapest_total_path->total_cost);
	else if (tree->parent == NULL)
		fprintf(f, "<br/><font point-size=\"7\">%.2f</font>",
					  previous_cost);
	fprintf(f, ">];\n");
}


static void
dump_query_tree_edge(Relids relids, Relids other_relids, FILE *f)
{
	fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	fprintf(f, ")\" -> \"(");
	fprintf_relids(f, other_relids);
	fprintf(f, ")\";\n");
}


static Relids
dump_query_tree_rec(PlannerInfo *root, QueryTree *tree,
					QueryTree *selected1, QueryTree *selected2,
					List *lselected1, List *lselected2,
					bool costs, FILE *f)
{
	Relids	relids_left, relids_right, relids;
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	Assert(tree != NULL);

	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		Assert(tree->rel != NULL);

		relids = bms_copy(tree->rel->relids);
		dump_query_tree_node(tree, selected1, selected2,
							 lselected1, lselected2,
							 relids, costs,
							 private->previous_cost, f);

		return relids;
	}

	Assert(tree->right != NULL);

	relids_left = dump_query_tree_rec(root, tree->left,
									  selected1, selected2,
									  lselected1, lselected2, costs, f);
	relids_right = dump_query_tree_rec(root, tree->right,
									   selected1, selected2,
									   lselected1, lselected2, costs, f);

	Assert(!bms_overlap(relids_left, relids_right));
	relids = bms_union(relids_left, relids_right);

	dump_query_tree_node(tree, selected1, selected2,
						 lselected1, lselected2,
						 relids, costs,
						 private->previous_cost, f);

	dump_query_tree_edge(relids, relids_left, f);
	dump_query_tree_edge(relids, relids_right, f);

	return relids;
}


void
dump_query_tree(PlannerInfo *root, QueryTree *tree, QueryTree *selected1,
				QueryTree *selected2, bool costs, char *path)
{
	dump_query_tree_list(root, tree, selected1, selected2, NIL, costs, path);
}


void
dump_query_tree_list(PlannerInfo *root, QueryTree *tree,
					 QueryTree *selected1, QueryTree *selected2,
					 List *selected, bool costs, char *path)
{
	dump_query_tree_list2(root, tree,
						  selected1, selected2, selected, NIL, costs, path);
}


void
dump_query_tree_list2(PlannerInfo *root, QueryTree *tree,
					  QueryTree *selected1, QueryTree *selected2,
					  List *lselected1, List *lselected2,
					  bool costs, char *path)
{
	FILE	*f;

	f = fopen(path, "w");
	fprintf(f, "strict digraph {\n");
	dump_query_tree_rec(root, tree, selected1, selected2,
						lselected1, lselected2, costs, f);
	fprintf(f, "}\n");
	fclose(f);
}


void
print_query_tree_list(char *intro, List *trees)
{
	ListCell	*lc;

	printf("%s", intro);
	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		printf("(");
		fprintf_relids(stdout, tree->rel->relids);
		printf(") ");
	}
	printf("\n");
}

void
trace_join(const char *path, RelOptInfo *r1, RelOptInfo *r2)
{
	FILE *f = fopen(path, "a");

	fprintf(f, "(");
	fprintf_relids(f, r1->relids);
	fprintf(f, ") JOIN (");
	fprintf_relids(f, r2->relids);
	fprintf(f, ")\n");
	fclose(f);
}

void
validate_tree(QueryTree *tree)
{
	if (tree == NULL)
		return;

	Assert(tree->rel != NULL);
	validate_tree(tree->left);
	validate_tree(tree->right);
}

void
validate_list(List *l)
{
	ListCell	*lc;
	ListCell	dereferenced;
	int			i;

	i = 0;
	printf("Validating list %p: ", l);
	foreach(lc, l)
	{
		printf("%d ", i++);
		fflush(stdout);
		dereferenced = *lc;
	}
	printf("\n");
}

#endif	/* SAIO_DEBUG */
