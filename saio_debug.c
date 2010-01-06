#include "postgres.h"

#include <stdio.h>

#include "optimizer/paths.h"
#include "saio_debug.h"


int debug_vprintf(FILE *f, const char *format, ...)
{
	int		i;
	va_list	arg;

	va_start(arg, format);
#ifdef DEBUG
	i = fprintf(f, format, arg);
#else
	i = 0;
#endif
	va_end(arg);
	return i;
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
			debug_fprintf(f, " ");
		debug_fprintf(f, "%d", x);
		first = false;
	}
	bms_free(tmprelids);
}


int debug_fprintf(FILE *f, const char *format, ...)
{
	int		i;
	va_list	arg;

	va_start(arg, format);
	i = debug_vprintf(f, format, arg);
	va_end(arg);
	return i;
}


int
debug_printf(const char *format, ...)
{
	int		i;
	va_list	arg;

	va_start(arg, format);
	i = debug_fprintf(stdout, format, arg);
	va_end(arg);
	return i;
}


static void
dump_query_tree_node(QueryTree *tree, QueryTree *selected1,
					 QueryTree *selected2, Relids relids, FILE *f)
{

	debug_fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	debug_fprintf(f, ")\"");
	if (tree == selected1)
		debug_fprintf(f, " [color=red, fontcolor=red, ");
	else if (tree == selected2)
		debug_fprintf(f, " [color=blue, fontcolor=blue, ");
	else
		debug_fprintf(f, " [");

	debug_fprintf(f, "label=<(");
	fprintf_relids(f, relids);
	debug_fprintf(f, ")");
	if (tree->rel)
		debug_fprintf(f, "<br/><font point-size=\"7\">%.2f</font>",
				tree->rel->cheapest_total_path->total_cost);
	debug_fprintf(f, ">];\n");
}


static void
dump_query_tree_edge(Relids relids, Relids other_relids, FILE *f)
{
	debug_fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	debug_fprintf(f, ")\" -> \"(");
	fprintf_relids(f, other_relids);
	debug_fprintf(f, ")\";\n");
}


static Relids
dump_query_tree_rec(PlannerInfo *root, QueryTree *tree,
					QueryTree *selected1, QueryTree *selected2, FILE *f)
{
	Relids	relids_left, relids_right, relids;

	Assert(tree != NULL);

	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		Assert(tree->rel != NULL);

		relids = bms_copy(tree->rel->relids);
		dump_query_tree_node(tree, selected1, selected2, relids, f);

		return relids;
	}

	Assert(tree->right != NULL);

	relids_left = dump_query_tree_rec(root, tree->left,
											selected1, selected2, f);
	relids_right = dump_query_tree_rec(root, tree->right,
											 selected1, selected2, f);

	Assert(!bms_overlap(relids_left, relids_right));
	relids = bms_union(relids_left, relids_right);

	dump_query_tree_node(tree, selected1, selected2, relids, f);

	dump_query_tree_edge(relids, relids_left, f);
	dump_query_tree_edge(relids, relids_right, f);

	return relids;
}


void
dump_query_tree(PlannerInfo *root, QueryTree *tree, QueryTree *selected1,
				QueryTree *selected2, char *path)
{
	FILE	*f;

	f = fopen(path, "w");
	debug_fprintf(f, "strict digraph {\n");
	dump_query_tree_rec(root, tree, selected1, selected2, f);
	debug_fprintf(f, "}\n");
	fclose(f);
}


void
print_query_tree_list(char *intro, List *trees)
{
	ListCell	*lc;

	debug_printf("%s", intro);
	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		debug_printf("(");
		fprintf_relids(stdout, tree->rel->relids);
		debug_printf(") ");
	}
	debug_printf("\n");
}

void
trace_join(const char *path, RelOptInfo *r1, RelOptInfo *r2)
{
	FILE *f = fopen(path, "a");

	debug_fprintf(f, "(");
	fprintf_relids(f, r1->relids);
	debug_fprintf(f, ") JOIN (");
	fprintf_relids(f, r2->relids);
	debug_fprintf(f, ")\n");
	fclose(f);
}
