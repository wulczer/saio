#include "postgres.h"

#include <stdio.h>
#include <printf.h>

#include "optimizer/paths.h"
#include "saio_debug.h"

#define printf(str, ...)


void
dump_debugging(SaioPrivateData *private)
{
	char path[1024];
	int suffix;

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

			fprintf(f, "%10.4f  %10.4f  %1d  %6d\n",
					step->temperature, step->cost,
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

	tree = *((QueryTree **) (args[0]));

	if (tree->rel == NULL)
	{
		len = asprintf(&buffer, "(nil)");
		if (len == -1)
			return -1;
		len = fprintf(stream, "%*s", (info->left ? -info->width : info->width),
                      buffer);
		free(buffer);
		return len;
	}

	buffer = malloc(bms_num_members(tree->rel->relids) * 4 + 2);
	if (!buffer)
		return -1;

	buffer[0] = '\0';
	tmprelids = bms_copy(tree->rel->relids);
	len = 0;
	while ((x = bms_first_member(tmprelids)) >= 0)
	{
		sprintf(buffer + len, first ? "(" : " ");
		len += 1;
		len += sprintf(buffer + len, "%d", x);
		first = false;
	}
	sprintf(buffer + len, ")");
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
					 List *selected, Relids relids, bool costs,
					 Cost previous_cost, FILE *f)
{

	fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	fprintf(f, ")\"");
	if (tree == selected1)
		fprintf(f, " [color=red, fontcolor=red, ");
	else if (tree == selected2)
		fprintf(f, " [color=blue, fontcolor=blue, ");
	else if (list_member_ptr(selected, tree))
		fprintf(f, " [color=green, fontcolor=green, ");
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
					List *selected, bool costs, FILE *f)
{
	Relids	relids_left, relids_right, relids;
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	Assert(tree != NULL);

	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		Assert(tree->rel != NULL);

		relids = bms_copy(tree->rel->relids);
		dump_query_tree_node(tree, selected1, selected2, selected,
							 relids, costs,
							 private->previous_cost, f);

		return relids;
	}

	Assert(tree->right != NULL);

	relids_left = dump_query_tree_rec(root, tree->left,
									  selected1, selected2,
									  selected, costs, f);
	relids_right = dump_query_tree_rec(root, tree->right,
									   selected1, selected2,
									   selected, costs, f);

	Assert(!bms_overlap(relids_left, relids_right));
	relids = bms_union(relids_left, relids_right);

	dump_query_tree_node(tree, selected1, selected2, selected,
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
	FILE	*f;

	f = fopen(path, "w");
	fprintf(f, "strict digraph {\n");
	dump_query_tree_rec(root, tree, selected1, selected2, selected, costs, f);
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
