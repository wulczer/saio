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

#include "postgres.h"

#include "utils/memutils.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"

#include "saio.h"
#include "saio_util.h"
#include "saio_trees.h"
#include "saio_debug.h"

/*
 * Merge a QueryTree into a list of QueryTrees by creating another tree that
 * will have the given one as one child and one of the existing ones as the
 * other child.
 *
 * Only considers creating joins that have join conditions, unless the force
 * parameter is true. Based heavily on GEQO's merge_clump(), but does not
 * maintain the invariant of having the list sorted by the size of the tree.
 *
 * Returns a new list of QueryTrees.
 */
static List *
merge_trees(PlannerInfo *root, List *result, QueryTree *tree, bool force)
{
	ListCell	*prev;
	ListCell	*lc;

	/* Go through the trees and find one to join the new one */
	prev = NULL;
	foreach(lc, result)
	{
		QueryTree	*other_tree = (QueryTree *) lfirst(lc);

		/* only choose ones that form a "desirable" join, unless forced */
		if (force || desirable_join(root, other_tree->rel, tree->rel))
		{
			RelOptInfo	*joinrel;

			/*
			 * Try joining the relations. This can fail because of join order
			 * restrictions or other restrictions.
			 */
			joinrel = make_join_rel(root, other_tree->rel, tree->rel);

			if (joinrel)
			{
				QueryTree	*new_tree;

				/* Managed to construct the join, build a new QueryTree */
				new_tree = (QueryTree *) palloc0(sizeof(QueryTree));

				/*
				 * The new tree's rel will be the relation that's just been
				 * built, and the children will be the just-merged tree and the
				 * one we took the other relation from.
				 * It doesn't matter which one is right and which one is left,
				 * because make_join_rel is symmetrical.
				 */
				new_tree->rel = joinrel;
				new_tree->left = other_tree;
				new_tree->right = tree;
				new_tree->parent = NULL;

				/* set the parent link, too */
				other_tree->parent = new_tree;
				tree->parent = new_tree;

				/* Find the cheapest path for the new relation */
				set_cheapest(joinrel);

				/* Remove the old tree from the list */
				result = list_delete_cell(result, lc, prev);

				/* Recursively merge the new tree into the list */
				return merge_trees(root, result, new_tree, force);
			}
		}
		prev = lc;
	}

	/* We don't care about the ordering, just add it at the beginning */
	return lcons(tree, result);
}


/*
 * Build a QueryTree from a list of RelOptInfos. The tree structure will
 * reflect the order in which the rels are joined.
 *
 * Heavily based on GEQO's gimme_tree(). Note however, that it gets called only
 * once in one SAIO algorithm execution.
 */
QueryTree *
make_query_tree(PlannerInfo *root, List *initial_rels)
{
	List		*result;
	ListCell	*lc;

	result = NIL;

	foreach(lc, initial_rels)
	{
		/* Get the next relation */
		RelOptInfo	*rel = (RelOptInfo *) lfirst(lc);
		QueryTree	*tree;

		/* Make it into a one-level tree */
		tree = (QueryTree *) palloc0(sizeof(QueryTree));
		tree->rel = rel;

		/* Merge it into the trees list using only desirable joins */
		result = merge_trees(root, result, tree, false);
	}

	if (list_length(result) > 1)
	{
		/* Force-join the remaining trees in some legal order */
		List	   *fresult;
		ListCell   *flc;

		fresult = NIL;
		foreach(flc, result)
		{
			QueryTree	*tree = (QueryTree *) lfirst(flc);

			fresult = merge_trees(root, fresult, tree, true);
		}
		result = fresult;
	}

	/* Did we succeed in forming a single join relation? */
	if (list_length(result) != 1)
		elog(ERROR, "failed to join all relations together");

	return (QueryTree *) linitial(result);
}


/* Recursive helper for get_all_nodes_rec() */
static List *
get_all_nodes_rec(QueryTree *tree, List *res)
{
	if (tree == NULL)
		return res;

	res = lcons(tree, res);
	get_all_nodes_rec(tree->left, res);
	get_all_nodes_rec(tree->right, res);
	return res;
}


/*
 * Return a list of all nodes of in a tree.
 */
List *
get_all_nodes(QueryTree *tree)
{
	return get_all_nodes_rec(tree, NIL);
}


/* Recursive helper for get_parents */
static List *
get_parents_rec(QueryTree *tree, bool reverse, List *res)
{
	Assert(tree != NULL);

	while (tree->parent != NULL)
	{
		if (reverse)
			res = lappend(res, tree->parent);
		else
			res = lcons(tree->parent, res);
		tree = tree->parent;
	}
	return res;
}


/*
 * Get all parents of a tree node, excluding the node itself.
 */
List *
get_parents(QueryTree *tree, bool reverse)
{
	return get_parents_rec(tree, reverse, NIL);
}


/*
 * Get all siblings of a tree node. Since QueryTrees are binary, this will
 * always be a one-element list.
 */
List *
get_siblings(QueryTree *tree)
{
	QueryTree	*parent;

	Assert(tree != NULL);

	parent = tree->parent;

	if (parent == NULL)
		return NIL;

	Assert(parent->left == tree || parent->right == tree);

	if (parent->left == tree)
		return list_make1(parent->right);

	/* has to be the left child */
	return list_make1(parent->left);
}


/*
 * Copy a query tree ignoring non-leaf rels. Used to transfer a tree to a
 * different memory context.
 */
QueryTree *
copy_tree_structure(QueryTree *tree)
{
	QueryTree *res;

	/* copying an empty tree is easy */
	if (tree == NULL)
		return NULL;

	/* tree is nonempty, create the current node */
	res = (QueryTree *) palloc(sizeof(QueryTree));

	/*
	 * Set the parent to NULL, for all nodes except the root node, it will be
	 * then set correctly by the higher-level recursive invocation.
	 */
	res->parent = NULL;

	/* for leaf nodes, copy the rel */
	if (tree->left == NULL)
		res->rel = tree->rel;

	/* copy the children */
	res->right = copy_tree_structure(tree->right);
	res->left = copy_tree_structure(tree->left);

	/* set the parent */
	if (res->right != NULL)
	{
		Assert(res->left != NULL);
		res->left->parent = res;
		res->right->parent = res;
	}

	return res;
}


List *
filter_leaves(List *trees)
{
	ListCell	*lc;
	List		*res = NIL;

	foreach(lc, trees)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		if (tree->left == NULL)
		{
			Assert(tree->right == NULL);
			continue;
		}

		res = lappend(res, tree);
	}

	return res;
}


/* Swap two subtrees around. */

void
swap_subtrees(QueryTree *tree1, QueryTree *tree2)
{
	QueryTree	*parent1, *parent2;

	/* can't swap the root node with anything */
	Assert(tree1->parent != NULL);
	Assert(tree2->parent != NULL);

	/*
	 * Since make_join_rel() is symmetrical, it doesn't make sense to swap
	 * sibling nodes.
	 */
	Assert(tree1->parent != tree2->parent);

	/* Do the swap */
	parent1 = tree1->parent;
	parent2 = tree2->parent;

	if (parent1->left == tree1)
		parent1->left = tree2;
	else
		parent1->right = tree2;

	if (parent2->left == tree2)
		parent2->left = tree1;
	else
		parent2->right = tree1;

	tree1->parent = parent2;
	tree2->parent = parent1;
}


/*
 * recalculate_tree_cutoff
 *    Rebuild the final relation in the order given by the input tree, assuming
 *    one subtree has already been processed.
 *
 * Traverses the QueryTree in postorder and recreates upper-level joinrels from
 * lower-level joinrels, starting at the leaves. Assumes the subtree pointed to
 * by cutoff is already computed and does not descend into it.
 *
 * If any of the intermidient joinrels cannot be constructed (because the tree
 * does not represent a valid join ordering) return false, otherwise return
 * true.
 *
 * If this function returns false, the tree is left in a dirty state, where the
 * values of "rel" pointers in the non-leaf nodes can contain bogus values.
 */
bool
recalculate_tree_cutoff_ctx(PlannerInfo *root, QueryTree *tree,
							QueryTree *cutoff, bool own_ctx)
{
	RelOptInfo	*joinrel;
	bool		ok;

	/* we should never be called on an empty tree */
	Assert(tree != NULL);

	/* if it's the root of the precomputed tree, we're done */
	if (tree == cutoff)
		return true;

	/* if it's a leaf, we're done */
	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		return true;
	}

	/* it's not a leaf */
	Assert(tree->right != NULL);

	/* recurse to the left child */
	ok = recalculate_tree_cutoff_ctx(root, tree->left, cutoff, own_ctx);

	/* it either failed or computed the left child's rel */
	Assert(!ok || (tree->left->rel != NULL));

	/* short-circuit the computation on failure*/
	if (!ok)
		return false;

	/* recurse to the right child */
	ok = recalculate_tree_cutoff_ctx(root, tree->right, cutoff, own_ctx);

	/* it either failed or computed the left child's rel */
	Assert(!ok || (tree->left->rel != NULL));

	/* short-circuit the computation on failure*/
	if (!ok)
		return false;

	elog(DEBUG1, "saio_trees: creating joinrel from %R and %R (own ctx: %d)\n",
		 tree->left->rel->relids, tree->right->rel->relids, own_ctx);

	/* try to join the children's relations */
	if (own_ctx)
	{
		MemoryContext old;
		old = MemoryContextSwitchTo(tree->ctx);
		joinrel = make_join_rel(root, tree->left->rel, tree->right->rel);
		MemoryContextSwitchTo(old);
	}
	else
	{
		joinrel = make_join_rel(root, tree->left->rel, tree->right->rel);
	}

	if (joinrel)
	{
		SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;
		private->joinrels_built++;

		/* constructed the joinrel, compute its paths and store it */
		set_cheapest(joinrel);
		tree->rel = joinrel;
		return true;
	}

	/* failed to build the joinrel */
	return false;
}


bool
recalculate_tree_cutoff(PlannerInfo *root, QueryTree *tree, QueryTree *cutoff)
{
	return recalculate_tree_cutoff_ctx(root, tree, cutoff, false);
}


/*
 * recalculate_tree
 *    Like recalculate_tree_cutoff but always traverses the whole tree.
 */
bool
recalculate_tree(PlannerInfo *root, QueryTree *tree)
{
	return recalculate_tree_cutoff(root, tree, NULL);
}


/*
 * Keep the previous state as a global minimum if:
 *  - the move is uphill
 *  - there is no global minimum or it is more expensive the the previous state
 *
 * This way we optimize the number of times we save a state, which means
 * copying the whole query tree.
 *
 * Also forget the global minimum if:
 *  - the move is downhill
 *  - there is a global mininum, but the previous state is cheaper
 */
void
keep_minimum_state(PlannerInfo *root, QueryTree *tree, Cost new_cost)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	if (private->previous_cost >= new_cost)
	{
		/* move is downhill */
		if ((private->min_tree != NULL) && (private->min_cost >= new_cost))
		{
			/* we broke the global minimum, forget it */
			elog(DEBUG1, "[%04d] global minimum broken\n", private->loop_no);
			MemoryContextReset(private->min_context);
			private->min_tree = NULL;
		}
	}
	else
	{
		/* move is uphill */
		if ((private->min_tree == NULL) ||
			(private->min_cost > private->previous_cost))
		{
			/* there is no global minimum, keep the current state */
			MemoryContext	old_context;

			if (private->min_tree != NULL)
				MemoryContextReset(private->min_context);

			elog(DEBUG1, "[%04d] new global minimum\n", private->loop_no);
			/* copy the current state into the global minimum context */
			old_context = MemoryContextSwitchTo(private->min_context);
			private->min_tree = copy_tree_structure(tree);
			MemoryContextSwitchTo(old_context);

			/* keep the cost as the minimal cost */
			private->min_cost = private->previous_cost;
		}
	}
}


#ifdef NOT_USED
static void
cleanup_tree(QueryTree *tree)
{
	if (tree->left == NULL)
	{
		Assert(tree->right == NULL);
		return;
	}

	tree->rel = NULL;
	cleanup_tree(tree->left);
	cleanup_tree(tree->right);
}
#endif
