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

#include <math.h>
#include <time.h>

#include "utils/memutils.h"
#include "nodes/pg_list.h"
#include "optimizer/paths.h"
#include "optimizer/pathnode.h"
#include "optimizer/joininfo.h"
#include "utils/hsearch.h"

#include "saio.h"

#ifdef SAIO_DEBUG
#include "saio_debug.h"
#else
#define printf(str, ...)
#define dump_query_tree_list2(root, tree, tree1, tree2, l1, l2, fake, path)
#define dump_query_tree_list(root, tree, tree1, tree2, l1, fake, path)
#define dump_query_tree(root, tree, tree1, tree2, fake, path);
#endif


#define SAIO_COST(rel) rel->cheapest_total_path->total_cost

#define OTHER_CHILD(node, child) (node)->left == (child) ? (node)->right : (node)->left

static char	path[256];


typedef struct JoinHashEntry
{
	Relids		join_relids;	/* hash key --- MUST BE FIRST */
	RelOptInfo *join_rel;
} JoinHashEntry;


/*
 * Save the current state of the variables that get modified during
 * make_join_rel(). Enter a temporary memory context that will get reset when
 * we leave the context. See geqo_eval() for similar code and explanations.
 */
static void
context_enter(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* join_rel_list and join_rel_hash get added to in make_join_rel() */
	private->savelength = list_length(root->join_rel_list);
	private->savehash = root->join_rel_hash;
	/* if a hash has already been built, we need to get rid of it */
	root->join_rel_hash = NULL;

	/* switch to the sketch context */
	private->old_context = MemoryContextSwitchTo(private->sketch_context);
}


/*
 * Restore the state, reset the sketch memory context.
 */
static void
context_exit(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* restore join_rel_list and join_rel_hash */
	root->join_rel_list = list_truncate(root->join_rel_list,
										private->savelength);
	root->join_rel_hash = private->savehash;

	/* switch back to the old context */
	MemoryContextSwitchTo(private->old_context);

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(private->sketch_context);
}


/* verbatim copy from geqo_eval.c */
static bool
desirable_join(PlannerInfo *root,
			   RelOptInfo *outer_rel, RelOptInfo *inner_rel)
{
	/*
	 * Join if there is an applicable join clause, or if there is a join order
	 * restriction forcing these rels to be joined.
	 */
	if (have_relevant_joinclause(root, outer_rel, inner_rel) ||
		have_join_order_restriction(root, outer_rel, inner_rel))
		return true;

	/* Otherwise postpone the join till later. */
	return false;
}


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
static QueryTree *
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
static List *
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
static List *
get_parents(QueryTree *tree, bool reverse)
{
	return get_parents_rec(tree, reverse, NIL);
}


/*
 * Get all siblings of a tree node. Since QueryTrees are binary, this will
 * always be a one-element list.
 */
static List *
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
static QueryTree *
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
static bool
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


static bool
recalculate_tree_cutoff(PlannerInfo *root, QueryTree *tree, QueryTree *cutoff)
{
	return recalculate_tree_cutoff_ctx(root, tree, cutoff, false);
}


/*
 * recalculate_tree
 *    Like recalculate_tree_cutoff but always traverses the whole tree.
 */
static bool
recalculate_tree(PlannerInfo *root, QueryTree *tree)
{
	return recalculate_tree_cutoff(root, tree, NULL);
}


/* Swap two subtrees around. */

static void
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


/* verbatim copy from geqo_random.c */
static void
initialize_random_state(PlannerInfo *root, double seed)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	if (seed == 0)
		seed = (double) time(NULL);

	memset(private->random_state, 0, sizeof(private->random_state));
	memcpy(private->random_state,
		   &seed,
		   Min(sizeof(private->random_state), sizeof(seed)));
}


static double
saio_rand(PlannerInfo *root)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	return erand48(private->random_state);
}


#define saio_randint(root, upper, lower) \
	( (int) floor( saio_rand(root)*(((upper)-(lower))+0.999999) ) + (lower) )


static bool
acceptable(PlannerInfo *root, Cost new_cost)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	/* downhill moves are always acceptable */
	if (new_cost < private->previous_cost)
		return true;

	/*
	 * If temperature < 1, moves that do not change state are considered
	 * unacceptable.
	 *
	 * FIXME: this is to avoid endless loop with the same temperature and state
	 * that is not changing, figure out *why* that happens and prevent it
	 */
	if ((private->temperature < 1) && (private->previous_cost == new_cost))
		return false;

	/*
	 * Uphill moves are acceptable with probability
	 *  exp((old - new) / temperature)
	 */
	return (saio_rand(root) < exp(
				((double) (private->previous_cost - new_cost))
				/ private->temperature));
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
static void
keep_minimum_state(PlannerInfo *root, QueryTree *tree, Cost new_cost)
{
	SaioPrivateData *private = (SaioPrivateData *) root->join_search_private;

	if (private->previous_cost >= new_cost)
	{
		/* move is downhill */
		if ((private->min_tree != NULL) && (private->min_cost >= new_cost))
		{
			/* we broke the global minimum, forget it */
			printf("[%04d] global minimum broken\n", private->loop_no);
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

			printf("[%04d] new global minimum\n", private->loop_no);
			/* copy the current state into the global minimum context */
			old_context = MemoryContextSwitchTo(private->min_context);
			private->min_tree = copy_tree_structure(tree);
			MemoryContextSwitchTo(old_context);

			/* keep the cost as the minimal cost */
			private->min_cost = private->previous_cost;
		}
	}
}


static List *
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



static void
context_enter_mem(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* switch to the sketch context */
	private->old_context = MemoryContextSwitchTo(private->sketch_context);
}


static void
context_exit_mem(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;
	MemoryContextSwitchTo(private->old_context);

	/* remove everything in the sketch context, but keep the context itself */
	MemoryContextResetAndDeleteChildren(private->sketch_context);
}


static bool
list_walker(List *l, bool (*walker) (QueryTree *, bool, void *),
			bool fake, void *extra_data)
{
	ListCell	*lc;

	foreach(lc, l)
	{
		QueryTree	*tree = (QueryTree *) lfirst(lc);

		if (!walker(tree, fake, extra_data))
			return false;
	}

	return true;
}


static bool
remove_from_planner(QueryTree *tree, bool fake, void *extra_data)
{
	PlannerInfo *root = (PlannerInfo *) extra_data;
	RelOptInfo	*rel = fake ? tree->tmp : tree->rel;

	printf("Removing %T from planner (fake: %d)\n", tree, fake);
	if (rel == NULL)
	{
		printf("Nothing to remove\n");
		return true;
	}

	Assert(list_member_ptr(root->join_rel_list, rel));
	root->join_rel_list = list_delete_ptr(
		root->join_rel_list, rel);

	if (root->join_rel_hash != NULL) {
		if (hash_search(root->join_rel_hash, &(rel->relids),
						HASH_REMOVE, NULL) == NULL)
			elog(ERROR, "join hash table corrupted");
	}

	return true;
}


static bool
reset_memory(QueryTree *tree, bool fake, void *extra_data)
{
	MemoryContext	ctx = fake ? tree->tmpctx : tree->ctx;

	printf("Resetting context %p (fake: %d)\n", ctx, fake);
	MemoryContextReset(ctx);
	return true;
}


static bool
nullify(QueryTree *tree, bool fake, void *extra_data)
{
	RelOptInfo		**rel = fake ? &tree->tmp : &tree->rel;

	printf("Nullifying relation %p (fake: %d)\n", *rel, fake);
	*rel = NULL;
	return true;
}


static void
rebuild_join_rel_hash(PlannerInfo *root)
{
	HTAB	   *hashtab;
	HASHCTL		hash_ctl;
	ListCell   *l;

	/* Create the hash table */
	MemSet(&hash_ctl, 0, sizeof(hash_ctl));
	hash_ctl.keysize = sizeof(Relids);
	hash_ctl.entrysize = sizeof(JoinHashEntry);
	hash_ctl.hash = bitmap_hash;
	hash_ctl.match = bitmap_match;
	hash_ctl.hcxt = CurrentMemoryContext;
	hashtab = hash_create("JoinRelHashTable",
						  256L,
						  &hash_ctl,
					HASH_ELEM | HASH_FUNCTION | HASH_COMPARE | HASH_CONTEXT);

	/* Insert all the already-existing joinrels */
	foreach(l, root->join_rel_list)
	{
		RelOptInfo *rel = (RelOptInfo *) lfirst(l);
		JoinHashEntry *hentry;
		bool		found;

		hentry = (JoinHashEntry *) hash_search(hashtab,
											   &(rel->relids),
											   HASH_ENTER,
											   &found);
		Assert(!found);
		hentry->join_rel = rel;
	}

	root->join_rel_hash = hashtab;
	printf("Created a joinrel hash %p\n", hashtab);
}


static bool
switch_contexts(QueryTree *tree, bool fake, void *extra_data)
{
	MemoryContext	tmpctx;
	RelOptInfo		*tmprel;

	tmpctx = tree->tmpctx;
	tree->tmpctx = tree->ctx;
	tree->ctx = tmpctx;

	tmprel = tree->tmp;
	tree->tmp = tree->rel;
	tree->rel = tmprel;

	printf("Switched contexts, rel is %p, tmp is %p\n", tree->rel, tree->tmp);

	return true;
}


static bool
recalculate(QueryTree *tree, bool fake, void *extra_data)
{
	PlannerInfo		*root = (PlannerInfo *) extra_data;
	SaioPrivateData	*private;
	RelOptInfo		*left, *right, *rel, **tree_rel;
	MemoryContext	ctx;
	ListCell		*prev, *cur;
	bool			had_no_hash;
	int				n;

	private = (SaioPrivateData *) root->join_search_private;

	printf("Recalculating tree from %T and %T (fake: %d)\n",
		   tree->left, tree->right, fake);

	Assert(tree->left != NULL);
	Assert(tree->right != NULL);

	tree_rel = fake ? &tree->tmp : &tree->rel;

	left = tree->left->tmp == NULL ? tree->left->rel : tree->left->tmp;
	right = tree->right->tmp == NULL ? tree->right->rel : tree->right->tmp;

	if (left == NULL)
		return false;
	if (right == NULL)
		return false;

	ctx = MemoryContextSwitchTo(fake ? tree->tmpctx : tree->ctx);
	n = list_length(root->join_rel_list);
	prev = list_tail(root->join_rel_list);
	had_no_hash = (root->join_rel_hash == NULL);
	rel = make_join_rel(root, left, right);
	MemoryContextSwitchTo(ctx);
	if (rel == NULL) {
		printf("Recalculation failed\n");
		return false;
	}
	private->joinrels_built++;
	Assert(list_length(root->join_rel_list) == n + 1);
	/* move the list cell to the correct memory context */
	cur = list_tail(root->join_rel_list);
	root->join_rel_list = list_delete_cell(root->join_rel_list, cur, prev);
	root->join_rel_list = lappend(root->join_rel_list, rel);
	/* move the rel hash to the correct memory context */
	if (had_no_hash && root->join_rel_hash != NULL)
		rebuild_join_rel_hash(root);
	*tree_rel = rel;
	set_cheapest(rel);
	return true;
}


static void
get_abc_paths(QueryTree *tree, QueryTree *tree1, QueryTree *tree2,
			  List **a, List **b, List **c)
{
	List *p1, *p2;

	p1 = get_parents(tree1, true);
	p2 = get_parents(tree2, true);

	*a = list_difference_ptr(p1, p2);
	*b = list_difference_ptr(p2, p1);
	*c = list_difference_ptr(p1, *a);

	list_free(p1);
	list_free(p2);
}


static int
swap_and_recalc(PlannerInfo *root, QueryTree *tree,
				QueryTree *tree1, QueryTree *tree2)
{
	List		*a, *b, *c, *ab;
	bool		ok;
	int			move_result;
	SaioPrivateData	*private;

	private = (SaioPrivateData *) root->join_search_private;

	swap_subtrees(tree1, tree2);

	get_abc_paths(tree, tree1, tree2, &a, &b, &c);
	ab = list_concat(a, b);

	printf("[%04d] Inside swap and recalc\n", private->loop_no);
	snprintf(path, 256, "/tmp/saio-recalc-%04d-inside.dot", private->loop_no);
	dump_query_tree_list2(root, tree, tree1, tree2, ab, c, true, path);


	/* Rebuild the A and B paths */
	ok = list_walker(ab, recalculate, true, (void *) root);

	/* If that failed, reset and remove the A and B paths and return */
	if (!ok)
	{
		printf("[%04d] Failed to rebuild AB paths\n", private->loop_no);
		list_walker(ab, remove_from_planner, true, (void *) root);
		list_walker(ab, reset_memory, true, (void *) root);
		list_walker(ab, nullify, true, (void *) root);
		swap_subtrees(tree1, tree2);
		snprintf(path, 256, "/tmp/saio-recalc-%04d-failed.dot", private->loop_no);
		dump_query_tree_list2(root, tree, tree1, tree2, ab, c, true, path);
		list_free(ab);
		list_free(c);
		return SAIO_MOVE_FAILED;
	}

	printf("[%04d] Managed to rebuild AB paths\n", private->loop_no);
	move_result = SAIO_MOVE_FAILED;
	/* Managed to rebuild A and B paths, now remove the C path and rebuild it */
	list_walker(c, remove_from_planner, false, (void *) root);
	list_walker(c, reset_memory, false, (void *) root);
	list_walker(c, nullify, false, (void *) root);
	ok = list_walker(c, recalculate, true, (void *) root);

	/* If managed to rebuild, check for acceptability */
	if (ok)
	{
		move_result = SAIO_MOVE_DISCARDED;
		printf("[%04d] Managed to rebuild C path\n", private->loop_no);
		ok = acceptable(root, SAIO_COST(tree->tmp));
	}

	/* If that failed, reset and remove the A and B paths, recalculate the C
	 * path and return */
	if (!ok)
	{
		printf("[%04d] Failed to rebuild the tree: "
			   "new cost %10.4f, old cost %10.4f, reason %d\n",
			   private->loop_no, SAIO_COST(tree->tmp), private->previous_cost,
			   move_result);
		list_walker(ab, remove_from_planner, true, (void *) root);
		list_walker(ab, reset_memory, true, (void *) root);
		list_walker(ab, nullify, true, (void *) root);
		list_walker(c, remove_from_planner, true, (void *) root);
		list_walker(c, reset_memory, true, (void *) root);
		list_walker(c, nullify, true, (void *) root);

		swap_subtrees(tree1, tree2);
		ok = list_walker(c, recalculate, false, (void *) root);
		Assert(ok);
		snprintf(path, 256, "/tmp/saio-recalc-%04d-failed.dot", private->loop_no);
		dump_query_tree_list2(root, tree, tree1, tree2, ab, c, true, path);
		list_free(ab);
		list_free(c);
		return move_result;
	}

	/* Everything has been rebuilt and is acceptabile */
	list_walker(ab, remove_from_planner, false, (void *) root);
	list_walker(ab, reset_memory, false, (void *) root);
	list_walker(ab, switch_contexts, false, (void *) root);
	list_walker(c, switch_contexts, false, (void *) root);
	list_walker(ab, nullify, true, (void *) root);
	list_walker(c, nullify, true, (void *) root);

	printf("[%04d] Success\n", private->loop_no);
	snprintf(path, 256, "/tmp/saio-recalc-%04d-success.dot", private->loop_no);
	dump_query_tree_list2(root, tree, tree1, tree2, ab, c, true, path);

	list_free(ab);
	list_free(c);
	return SAIO_MOVE_OK;
}


static int
saio_recalc_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List			*choices, *tmp;
	QueryTree		*tree1, *tree2;
	SaioPrivateData	*private;
	bool			ok;
	int				move_result = SAIO_MOVE_FAILED;

	if (MemoryContextIsEmpty(tree->ctx)) {
		/* rebuild the tree putting each node in its own context */
		printf("Rebuilding the tree in separate contexts\n");
		ok = recalculate_tree_cutoff_ctx(root, tree, NULL, true);
		Assert(ok);
		if (root->join_rel_hash != NULL)
		{
			printf("Had to rebuilding the joinrel hash as well\n");
			rebuild_join_rel_hash(root);
		}
	}

	private = (SaioPrivateData *) root->join_search_private;

	/* if less than four trees to choose from, return immediately */
	if (list_length(all_trees) < 4)
	{
		printf("Less than four tress to choose from, exiting\n");
		return SAIO_MOVE_IMPOSSIBLE;
	}

	context_enter_mem(root);

	choices = list_copy(all_trees);
	choices = list_delete_ptr(choices, llast(choices));

	tree1 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	tmp = get_all_nodes(tree1);
	tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
	tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
	choices = list_difference_ptr(choices, tmp);

	if (choices == NIL)
	{
		context_exit_mem(root);
		printf("Could not find suitable choice, exiting\n");
		return SAIO_MOVE_IMPOSSIBLE;
	}

	tree2 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	printf("[%04d] Starting recalc move\n", private->loop_no);
	snprintf(path, 256, "/tmp/saio-recalc-%04d-try.dot", private->loop_no);
	dump_query_tree_list(root, tree, tree1, tree2, choices, true, path);

	context_exit_mem(root);

	move_result = swap_and_recalc(root, tree, tree1, tree2);

	if (move_result != SAIO_MOVE_OK)
	{
		printf("[%04d] Swap and recalc failed with %d\n",
			   private->loop_no, move_result);
		snprintf(path, 256, "/tmp/saio-recalc-%04d-failed.dot", private->loop_no);
		dump_query_tree_list(root, tree, tree1, tree2, NIL, true, path);
		return move_result;
	}

	/* swap the subtrees to temporarily go back to the previous state */
	swap_subtrees(tree1, tree2);
	keep_minimum_state(root, tree, SAIO_COST(tree->rel));
	private->previous_cost = SAIO_COST(tree->rel);
	swap_subtrees(tree1, tree2);

	return SAIO_MOVE_OK;
}


#ifdef NOT_USED
static bool
pivot_is_possible(PlannerInfo *root, QueryTree *pivot_root)
{
	QueryTree	*a, *b, *c;
	RelOptInfo	*r1, *r2;

	a = pivot_root->left;
	b = pivot_root->right;
	c = OTHER_CHILD(pivot_root->parent, pivot_root);

	r1 = make_join_rel(root, b->rel, c->rel);
	if (r1 == NULL)
		return false;

	set_cheapest(r1);

	r2 = make_join_rel(root, a->rel, r1);
	if (r2 == NULL)
		return false;

	set_cheapest(r2);

	return true;
}
#endif


static void
execute_pivot(QueryTree *pivot_root)
{
	QueryTree	*a, *b, *c;

	a = pivot_root->left;
	b = pivot_root->right;
	c = OTHER_CHILD(pivot_root->parent, pivot_root);

	swap_subtrees(a, c);
}


static bool
saio_pivot_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices;
	QueryTree	*pivot_root;
	SaioPrivateData	*private;

	/* if less than four trees to choose from, return immediately */
	if (list_length(all_trees) < 4)
		return false;

	private = (SaioPrivateData *) root->join_search_private;

	/* get all trees */
	choices = list_copy(all_trees);
	/* remove root */
	choices = list_delete_ptr(choices, llast(choices));
	/* remove leaves */
	choices = filter_leaves(choices);

	while (choices != NIL)
	{
		bool	ok;
		Cost	new_cost;

		/* pick pivot root */
		pivot_root = list_nth(choices, saio_randint(
								  root, 0, list_length(choices) - 1));
		choices = list_delete_ptr(choices, pivot_root);

		Assert(pivot_root->left != NULL);
		Assert(pivot_root->right != NULL);
		Assert(pivot_root->parent != NULL);

		snprintf(path, 256, "/tmp/saio-pivot-%04d-try.dot", private->loop_no);
		dump_query_tree_list(root, tree, pivot_root, NULL, choices, false, path);

		execute_pivot(pivot_root);

		context_enter(root);

		ok = recalculate_tree(root, pivot_root->parent);

		if (!ok)
		{
			execute_pivot(pivot_root);
			context_exit(root);
			continue;
		}

		ok = recalculate_tree_cutoff(root, tree, pivot_root->parent);
		Assert(ok);

		new_cost = SAIO_COST(tree->rel);

		ok = acceptable(root, new_cost);

		if (!ok)
		{
			snprintf(path, 256, "/tmp/saio-pivot-%04d-failed.dot", private->loop_no);
			dump_query_tree_list(root, tree, pivot_root, NULL, choices, true, path);
			execute_pivot(pivot_root);
			context_exit(root);
			continue;
		}

		snprintf(path, 256, "/tmp/saio-pivot-%04d-successful.dot", private->loop_no);
		dump_query_tree_list(root, tree, pivot_root, NULL, choices, true, path);
		keep_minimum_state(root, tree, new_cost);
		private->previous_cost = new_cost;

		context_exit(root);
		list_free(choices);
		return true;
	}

	list_free(choices);
	return false;
}


static bool
saio_move(PlannerInfo *root, QueryTree *tree, List *all_trees)
{
	List		*choices, *tmp;
	QueryTree	*tree1, *tree2;
	Cost		new_cost;
	bool		ok;
	SaioPrivateData	*private;

	/* if less than four trees to choose from, return immediately */
	if (list_length(all_trees) < 4)
		return false;

	private = (SaioPrivateData *) root->join_search_private;

	context_enter(root);

	choices = list_copy(all_trees);
	choices = list_delete_ptr(choices, llast(choices));

	tree1 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	tmp = get_all_nodes(tree1);
	tmp = list_concat_unique_ptr(get_parents(tree1, false), tmp);
	tmp = list_concat_unique_ptr(get_siblings(tree1), tmp);
	choices = list_difference_ptr(choices, tmp);

	snprintf(path, 256, "/tmp/saio-move-%04d-try.dot", private->loop_no);
	dump_query_tree_list(root, tree, tree1, NULL, choices, false, path);

	if (choices == NIL)
	{
		context_exit(root);
		return false;
	}

	tree2 = list_nth(choices, saio_randint(root, 0, list_length(choices) - 1));

	swap_subtrees(tree1, tree2);

	ok = recalculate_tree(root, tree);

	if (!ok)
	{
		swap_subtrees(tree1, tree2);
		context_exit(root);
		return false;
	}

	new_cost = SAIO_COST(tree->rel);

	ok = acceptable(root, new_cost);

	if (!ok)
	{
		snprintf(path, 256, "/tmp/saio-move-%04d-failed.dot", private->loop_no);
		dump_query_tree(root, tree, tree1, tree2, true, path);
		swap_subtrees(tree1, tree2);
		context_exit(root);
		return false;
	}

	snprintf(path, 256, "/tmp/saio-move-%04d-successful.dot", private->loop_no);
	dump_query_tree(root, tree, tree1, tree2, true, path);

	keep_minimum_state(root, tree, new_cost);
	private->previous_cost = new_cost;

	context_exit(root);

	return true;
}


static bool
equilibrium(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->elapsed_loops++;

	if (private->elapsed_loops >= private->equilibrium_loops)
	{
		/* reset the elapsed loops count */
		private->elapsed_loops = 0;
		/* equilibrium reached */
		return true;
	}

	return false;
}


static void
reduce_temperature(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	private->temperature *= saio_temperature_reduction_factor;
}


static bool
frozen(PlannerInfo *root)
{
	SaioPrivateData	*private = (SaioPrivateData *) root->join_search_private;

	/* can only be frozen when temperature < 1 */
	if (private->temperature > 1)
		return false;

	/* check the number of consecutive failed moves */
	return private->failed_moves >= saio_moves_before_frozen;
}


static void
init_tree_contexts(QueryTree *tree)
{
	Assert(tree != NULL);

	tree->ctx = AllocSetContextCreate(CurrentMemoryContext, "SAIO node",
									  ALLOCSET_DEFAULT_MINSIZE,
									  ALLOCSET_DEFAULT_INITSIZE,
									  ALLOCSET_DEFAULT_MAXSIZE);
	tree->tmpctx = AllocSetContextCreate(CurrentMemoryContext, "SAIO tmp node",
										 ALLOCSET_DEFAULT_MINSIZE,
										 ALLOCSET_DEFAULT_INITSIZE,
										 ALLOCSET_DEFAULT_MAXSIZE);
	if (tree->right != NULL)
		init_tree_contexts(tree->right);
	if (tree->left != NULL)
		init_tree_contexts(tree->left);
}


RelOptInfo *
saio(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	QueryTree		*tree;
	List			*all_trees;
	RelOptInfo		*res;
	SaioPrivateData	private;

	/* Initialize private data */
	root->join_search_private = (void *) &private;

	/* Initialize the random state */
	initialize_random_state(root, saio_seed);

	/*
	 * Create a sketch memory context as a child of the current context, so it
	 * gets cleaned automatically in case of a ereport(ERROR) exit.
	 */
	private.sketch_context = AllocSetContextCreate(CurrentMemoryContext,
												   "SAIO",
												   ALLOCSET_DEFAULT_MINSIZE,
												   ALLOCSET_DEFAULT_INITSIZE,
												   ALLOCSET_DEFAULT_MAXSIZE);

	/* Create a context for keeping the minimum state */
	private.min_context = AllocSetContextCreate(CurrentMemoryContext,
												"SAIO min",
												ALLOCSET_DEFAULT_MINSIZE,
												ALLOCSET_DEFAULT_INITSIZE,
												ALLOCSET_DEFAULT_MAXSIZE);
	/*
	 * Build a query tree from the initial relations. This should a tree that
	 * represents any valid join order for the given set of rels.
	 * Do it in a sketch context to avoid polluting root->join_rel_list and
	 * root->join_rel_hash and to be able to free the memory taken by
	 * constructing paths after determining an initial join order.
	 */
	if (saio_move_algorithm != SAIO_ALGORITHM_RECALC)
		context_enter(root);

	tree = make_query_tree(root, initial_rels);
	/* Set the initial tree cost */
	private.previous_cost = SAIO_COST(tree->rel);

	/*
	 * Copy the tree structure to the correct memory context. The rest of the
	 * memory allocated in make_query_tree() will get freed in context_exit().
	 */
	if (saio_move_algorithm != SAIO_ALGORITHM_RECALC)
	{
		MemoryContextSwitchTo(private.old_context);
		tree = copy_tree_structure(tree);
		MemoryContextSwitchTo(private.sketch_context);

		context_exit(root);
	}

	/* Set the number of loops before considering equilibrium */
	private.equilibrium_loops = levels_needed * saio_equilibrium_factor;
	/* Set the initial temperature */
	private.temperature = (double) private.previous_cost;
	private.temperature *= saio_initial_temperature_factor;
	/* Initialize the elapsed loops and failed moves counters */
	private.elapsed_loops = 0;
	private.failed_moves = 0;
	/* Initialize the minimal state */
	private.min_tree = NULL;


	/* init debugging */
	private.steps = NIL;
	private.joinrels_built = 0;
	private.loop_no = 0;

	/* if the algorithm is RECALC, init the contexts */
	if (saio_move_algorithm == SAIO_ALGORITHM_RECALC)
		init_tree_contexts(tree);

	/*
	 * Get the list of all trees to then pick randomly from them when doing SA
	 * algorithm moves.
	 */
	all_trees = get_all_nodes(tree);

	do {

		do {
			int			move_result = 0;
#ifdef SAIO_DEBUG
			/* save values for debugging */
			SaioStep	*step = palloc(sizeof(SaioStep));

			step->cost = private.previous_cost;
			step->temperature = private.temperature;
			step->joinrels_built = private.joinrels_built;
			private.joinrels_built = 0;
#endif
			switch (saio_move_algorithm)
			{
				case SAIO_ALGORITHM_MOVE:
					move_result = saio_move(root, tree, all_trees);
					break;
				case SAIO_ALGORITHM_PIVOT:
					move_result = saio_pivot_move(root, tree, all_trees);
					break;
				case SAIO_ALGORITHM_RECALC:
					move_result = saio_recalc_move(root, tree, all_trees);
					break;
				default:
					elog(ERROR, "invalid algorithm: %d", saio_move_algorithm);
			}
			if (move_result == 0)
			{
#ifdef SAIO_DEBUG
				step->move_result = 0;
#endif
				private.loop_no++;
				private.failed_moves = 0;
			}
			else
			{
#ifdef SAIO_DEBUG
				step->move_result = move_result;
#endif
				private.loop_no++;
				private.failed_moves++;
			}
#ifdef SAIO_DEBUG
			private.steps = lappend(private.steps, step);
#endif
			printf("[%04d] at the end of the loop min tree is %p with cost %10.4f\n",
				   private.loop_no, private.min_tree,
				   private.min_tree == NULL ? 0 : private.min_cost);

		} while (!equilibrium(root));

		reduce_temperature(root);

	} while (!frozen(root));

#ifdef SAIO_DEBUG
	/* dump debugging values, free memory */
	dump_debugging(&private);
	list_free_deep(private.steps);
#endif

	printf("[%04d] at the end of the algorithm min tree is %p with cost %10.4f\n",
		   private.loop_no, private.min_tree,
		   private.min_tree == NULL ? 0 : private.min_cost);

	/* if there is a global minimum, pick it */
	if (private.min_tree != NULL)
	{
		tree = private.min_tree;
		printf("The cheapest tree is %10.4f\n", private.min_cost);
	}

	/* Rebuild the final rel in the correct memory context */
	recalculate_tree(root, tree);
	snprintf(path, 256, "/tmp/saio-final.dot");
	dump_query_tree(root, tree, NULL, NULL, true, path);
	res = tree->rel;

	/* Clean up */
	list_free(all_trees);
	MemoryContextDelete(private.sketch_context);
	MemoryContextDelete(private.min_context);
	root->join_search_private = NULL;
	fflush(stdout);

	return res;
}
