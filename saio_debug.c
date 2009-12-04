#include "postgres.h"

#include <stdio.h>

#include "nodes/print.h"
#include "optimizer/paths.h"
#include "saio_debug.h"


static void
print_relids(Relids relids)
{
	Relids		tmprelids;
	int			x;
	bool		first = true;

	tmprelids = bms_copy(relids);
	while ((x = bms_first_member(tmprelids)) >= 0)
	{
		if (!first)
			printf(" ");
		printf("%d", x);
		first = false;
	}
	bms_free(tmprelids);
}


static void
print_restrictclauses(PlannerInfo *root, List *clauses)
{
	ListCell   *l;

	foreach(l, clauses)
	{
		RestrictInfo *c = lfirst(l);

		print_expr((Node *) c->clause, root->parse->rtable);
		if (lnext(l))
			printf(", ");
	}
}


static void
print_path(PlannerInfo *root, Path *path, int indent)
{
	const char *ptype;
	bool		join = false;
	Path	   *subpath = NULL;
	int			i;

	switch (nodeTag(path))
	{
		case T_Path:
			ptype = "SeqScan";
			break;
		case T_IndexPath:
			ptype = "IdxScan";
			break;
		case T_BitmapHeapPath:
			ptype = "BitmapHeapScan";
			break;
		case T_BitmapAndPath:
			ptype = "BitmapAndPath";
			break;
		case T_BitmapOrPath:
			ptype = "BitmapOrPath";
			break;
		case T_TidPath:
			ptype = "TidScan";
			break;
		case T_AppendPath:
			ptype = "Append";
			break;
		case T_ResultPath:
			ptype = "Result";
			break;
		case T_MaterialPath:
			ptype = "Material";
			subpath = ((MaterialPath *) path)->subpath;
			break;
		case T_UniquePath:
			ptype = "Unique";
			subpath = ((UniquePath *) path)->subpath;
			break;
		case T_NoOpPath:
			ptype = "NoOp";
			subpath = ((NoOpPath *) path)->subpath;
			break;
		case T_NestPath:
			ptype = "NestLoop";
			join = true;
			break;
		case T_MergePath:
			ptype = "MergeJoin";
			join = true;
			break;
		case T_HashPath:
			ptype = "HashJoin";
			join = true;
			break;
		default:
			ptype = "???Path";
			break;
	}

	for (i = 0; i < indent; i++)
		printf("\t");
	printf("%s", ptype);

	if (path->parent)
	{
		printf("(");
		print_relids(path->parent->relids);
		printf(") rows=%.0f", path->parent->rows);
	}
	printf(" cost=%.2f..%.2f\n", path->startup_cost, path->total_cost);

	if (path->pathkeys)
	{
		for (i = 0; i < indent; i++)
			printf("\t");
		printf("  pathkeys: ");
		print_pathkeys(path->pathkeys, root->parse->rtable);
	}

	if (join)
	{
		JoinPath   *jp = (JoinPath *) path;

		for (i = 0; i < indent; i++)
			printf("\t");
		printf("  clauses: ");
		print_restrictclauses(root, jp->joinrestrictinfo);
		printf("\n");

		if (IsA(path, MergePath))
		{
			MergePath  *mp = (MergePath *) path;

			for (i = 0; i < indent; i++)
				printf("\t");
			printf("  sortouter=%d sortinner=%d materializeinner=%d\n",
				   ((mp->outersortkeys) ? 1 : 0),
				   ((mp->innersortkeys) ? 1 : 0),
				   ((mp->materialize_inner) ? 1 : 0));
		}

		print_path(root, jp->outerjoinpath, indent + 1);
		print_path(root, jp->innerjoinpath, indent + 1);
	}

	if (subpath)
		print_path(root, subpath, indent + 1);
}


void
debug_print_rel(PlannerInfo *root, RelOptInfo *rel)
{
	ListCell   *l;

	printf("RELOPTINFO (");
	print_relids(rel->relids);
	printf("): rows=%.0f width=%d\n", rel->rows, rel->width);

	if (rel->baserestrictinfo)
	{
		printf("\tbaserestrictinfo: ");
		print_restrictclauses(root, rel->baserestrictinfo);
		printf("\n");
	}

	if (rel->joininfo)
	{
		printf("\tjoininfo: ");
		print_restrictclauses(root, rel->joininfo);
		printf("\n");
	}

	printf("\tpath list:\n");
	foreach(l, rel->pathlist)
		print_path(root, lfirst(l), 1);
	printf("\n\tcheapest startup path:\n");
	print_path(root, rel->cheapest_startup_path, 1);
	printf("\n\tcheapest total path:\n");
	print_path(root, rel->cheapest_total_path, 1);
	printf("\n");
	fflush(stdout);
}


void
fprint_relids(FILE *f, Relids relids)
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
debug_dump_query_tree_rec(PlannerInfo *root, QueryTree *tree,
						  QueryTree *selected1, QueryTree *selected2,
						  FILE *f)
{
	Assert(tree != NULL);
	Assert(tree->rel != NULL);

	fprintf(f, "        \"(");
	fprint_relids(f, tree->rel->relids);
	fprintf(f, ")\"");
	if (tree == selected1)
		fprintf(f, " [color=red, fontcolor=red]");
	else if (tree == selected2)
		fprintf(f, " [color=blue, fontcolor=blue]");
	fprintf(f, ";\n");

	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		return;
	}
	Assert(tree->right != NULL);

	fprintf(f, "        \"(");
	fprint_relids(f, tree->rel->relids);
	fprintf(f, ")\" -> \"(");
	fprint_relids(f, tree->left->rel->relids);
	fprintf(f, ")\"\n");

	fprintf(f, "        \"(");
	fprint_relids(f, tree->rel->relids);
	fprintf(f, ")\" -> \"(");
	fprint_relids(f, tree->right->rel->relids);
	fprintf(f, ")\";\n");

	debug_dump_query_tree_rec(root, tree->left, selected1, selected2, f);
	debug_dump_query_tree_rec(root, tree->right, selected1, selected2, f);
}

void
debug_dump_query_tree(PlannerInfo *root, QueryTree *tree,
					  QueryTree *selected1, QueryTree *selected2,
					  char *path)
{
	FILE	*f;

	f = fopen(path, "w");
	fprintf(f, "strict digraph {\n");
	debug_dump_query_tree_rec(root, tree, selected1, selected2, f);
	fprintf(f, "}\n");
	fclose(f);
}

static void
debug_verify_query_tree_rec(QueryTree *tree, QueryTree *left, QueryTree *right)
{
	if (left == NULL)
	{
		Assert(right == NULL);
		return;
	}
	Assert(right != NULL);

	Assert(left->parent == tree);
	Assert(right->parent == tree);

	debug_verify_query_tree_rec(left, left->left, left->right);
	debug_verify_query_tree_rec(right, right->left, right->right);
}

void
debug_verify_query_tree(QueryTree *tree)
{
	Assert(tree->parent == NULL);
	debug_verify_query_tree_rec(tree, tree->left, tree->right);
}

void
debug_print_query_tree_list(char *intro, List *trees)
{
	ListCell	*lc;

	printf("%s", intro);
	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		printf("(");
		fprint_relids(stdout, tree->rel->relids);
		printf(") ");
	}
	printf("\n");
}
