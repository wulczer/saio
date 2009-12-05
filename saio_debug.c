#include "postgres.h"

#include <stdio.h>

#include "optimizer/paths.h"
#include "saio_debug.h"


void
fprintf_relids(FILE *f, Relids relids)
{
	Relids		tmprelids;
	int			x;
	bool		first = true;

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
dump_query_tree_node(QueryTree *tree, QueryTree *selected1,
					 QueryTree *selected2, Relids relids, FILE *f)
{

	fprintf(f, "        \"(");
	fprintf_relids(f, relids);
	fprintf(f, ")\"");
	if (tree == selected1)
		fprintf(f, " [color=red, fontcolor=red, ");
	else if (tree == selected2)
		fprintf(f, " [color=blue, fontcolor=blue, ");
	else
		fprintf(f, " [");

	fprintf(f, "label=<(");
	fprintf_relids(f, relids);
	fprintf(f, ")");
	if (tree->rel)
		fprintf(f, "<br/><font point-size=\"7\">%.2f</font>",
				tree->rel->cheapest_total_path->total_cost);
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
	fprintf(f, "strict digraph {\n");
	dump_query_tree_rec(root, tree, selected1, selected2, f);
	fprintf(f, "}\n");
	fclose(f);
}


static void
verify_query_tree_rec(QueryTree *tree, QueryTree *left, QueryTree *right)
{
	if (left == NULL)
	{
		Assert(right == NULL);
		Assert(tree->rel != NULL);
		return;
	}
	Assert(right != NULL);
	Assert(tree->rel == NULL);

	Assert(left->parent == tree);
	Assert(right->parent == tree);

	verify_query_tree_rec(left, left->left, left->right);
	verify_query_tree_rec(right, right->left, right->right);
}


void
verify_query_tree(QueryTree *tree)
{
	Assert(tree->parent == NULL);
	verify_query_tree_rec(tree, tree->left, tree->right);
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
