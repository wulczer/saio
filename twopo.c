/*
 * twopo.c
 *    Two Phase Optimization
 *
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * contributed by:
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *  Adriano Lange                  *  C3SL - Centro de ComputaÃ§Ã£o    *
 *  adriano@c3sl.ufpr.br           *  CientÃ­fica e Software Livre /  *
 *                                 *  Departamento de InformÃ¡tica /  *
 *                                 *  Universidade Federal do ParanÃ¡ *
 *                                 *  Curitiba, Brasil               *
 *                                 *                                 *
 *                                 *  http://www.c3sl.ufpr.br        *
 *                                 *                                 *
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *
 * Implementation based on:
 *   Y. E. Ioannidis and Y. Kang, "Randomized algorithms for optimizing
 *   large join queries," SIGMOD Rec., vol. 19, no. 2, pp. 312â321, 1990.
 */

#include "postgres.h"

#include <math.h>

#include "optimizer/twopo.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/joininfo.h"
#include "parser/parsetree.h"
#include "utils/memutils.h"

//#define TWOPO_DEBUG

#define TWOPO_CACHE_PLANS

#define nodeCost(node) node->rel->cheapest_total_path->total_cost

#define swapValues(type,v1,v2) \
	{ \
		type aux = v1; \
		v1 = v2; \
		v2 = aux; \
	}

// heuristic initial states (see makeInitialState())
bool   twopo_heuristic_states          = DEFAULT_TWOPO_HEURISTIC_STATES;
// number of initial states in Iterative Improvement (II) phase
int    twopo_ii_stop                   = DEFAULT_TWOPO_II_STOP;
// enable Simulated Annealing (SA) phase
bool   twopo_sa_phase                  = DEFAULT_TWOPO_SA_PHASE;
// SA initial temperature: T = X * cost( min_state_from_ii_phase )
double twopo_sa_initial_temperature    = DEFAULT_TWOPO_SA_INITIAL_TEMPERATURE;
// SA temperature reduction: Tnew = X * Told
double twopo_sa_temperature_reduction  = DEFAULT_TWOPO_SA_TEMPERATURE_REDUCTION;
// SA inner loop equilibrium: for( i=0; i < E * Joins ; i++ )
int    twopo_sa_equilibrium            = DEFAULT_TWOPO_SA_EQUILIBRIUM;

/**
 * treeNode:
 *    Optimizer's main struct.
 *    Represent either a base relation or a binary join operation.
 *    It has cache support (see joinNodes()).
 */
typedef struct treeNode {
	RelOptInfo         *rel;
	List               *parents;
	struct treeNode    *inner_child;
	struct treeNode    *outer_child;
	// only for two-level nodes: (used in buildBushyTree())
	int                 head_idx;
	int                 tail_idx;
} treeNode;

/**
 * tempCtx:
 *    Temporary memory context struct.
 *    Store main informations needed context switch.
 */
typedef struct tempCtx {
	MemoryContext  mycontext;
	MemoryContext  oldcxt;
	int            savelength;
	struct HTAB   *savehash;
} tempCtx;

static treeNode*
createTreeNodes( int items )
{
	return (treeNode*) palloc0(items * sizeof(treeNode));
}

static treeNode*
buildOneLevelNodes( List *initial_rels, int levels_needed )
{
	int          i = 0;
	ListCell    *x;
	RelOptInfo  *rel;
	treeNode    *oneLevelNodes = createTreeNodes( levels_needed );

	foreach(x, initial_rels)
	{
		rel = (RelOptInfo *) lfirst(x);
		oneLevelNodes[i++].rel = rel;
	}

	return oneLevelNodes;
}

static treeNode*
joinNodes( PlannerInfo *root, treeNode *inner_node, treeNode *outer_node )
{
	treeNode *new_node = NULL;
	RelOptInfo *jrel;

#	ifdef TWOPO_CACHE_PLANS
	if ( inner_node->parents ) {
		ListCell *x;
		treeNode *node;
		foreach( x, inner_node->parents )
		{
			node = lfirst(x);
			if( node->inner_child == outer_node
				|| node->outer_child == outer_node )
			{
				new_node = node;
				break;
			}
		}
	}
#	endif

	if ( ! new_node ) {
		if( bms_overlap(inner_node->rel->relids, outer_node->rel->relids ) ){
			elog( ERROR, "joinNodes(): Overlap error!");
		}
		jrel = make_join_rel(root, inner_node->rel, outer_node->rel);
		if (jrel) {
			set_cheapest( jrel );
			new_node = createTreeNodes(1);
			new_node->rel = jrel;
			new_node->inner_child = inner_node;
			new_node->outer_child = outer_node;
			inner_node->parents = lcons(new_node,
					inner_node->parents);
			outer_node->parents = lcons(new_node,
					outer_node->parents);
		}

	}

	return new_node;
}

static List*
buildTwoLevelNodes( PlannerInfo *root,
		treeNode *oneLevelNodes, int numOneLevelNodes)
{
	treeNode    *new_node;
	List        *twolevelnodes = NULL;
	int          i,j;
	RelOptInfo  *rel1,
	            *rel2;

	for( i=0; i<numOneLevelNodes; i++ ) {
		rel1 = oneLevelNodes[i].rel;

		for( j=i; j<numOneLevelNodes; j++ ) {
			rel2 = oneLevelNodes[j].rel;

			if (!bms_overlap(rel1->relids, rel2->relids) &&
				(have_relevant_joinclause(root, rel1, rel2) ||
				have_join_order_restriction(root, rel1, rel2)))
			{
				new_node = joinNodes( root, oneLevelNodes+i, oneLevelNodes+j );
				if( new_node ){
					new_node->head_idx = i;
					new_node->tail_idx = j;
					twolevelnodes = lcons( new_node, twolevelnodes );
				}
			}
		}
		if( ! oneLevelNodes[i].parents ) {
			for( j=0; j<numOneLevelNodes; j++ ) {
				if( i == j )
					continue;
				rel2 = oneLevelNodes[j].rel;

				new_node = joinNodes( root, oneLevelNodes+i, oneLevelNodes+j );
				if( new_node ){
					new_node->head_idx = i;
					new_node->tail_idx = j;
					twolevelnodes = lcons( new_node, twolevelnodes );
				}
			}
		}
	}

	return twolevelnodes;
}

static inline int
find_root(int idx, int *parent_list)
{
	while( parent_list[idx] != idx )
		idx = parent_list[idx];

	return idx;
}

static void
join_trees(int *root1, int *root2, int *weight, int *parent, int *numTrees)
{
	if( weight[*root2]>weight[*root1] ){
		swapValues(int, *root1, *root2 );
	}
	weight[*root1] += weight[*root2];
	parent[*root2] = parent[*root1];
	(*numTrees)--;
}

static treeNode*
buildBushyTree(PlannerInfo *root, treeNode *leafNodes, int numLeaves,
                                  treeNode **edgeList, int numEdges)
{
	treeNode   **subtrees; /* partial plans of each tree */
	int 	     i,
	             numSubtrees, /* number of trees */
	            *parent, /* parent list. Used for tree detection */
	            *weight, /* weight list. Used to decide the new root in join
	                      * trees */
	             last_join = -1; /* finally, point the index of final plan in
	                              * subplan_list */

	parent = (int*) palloc((numLeaves) * sizeof(int));
	weight = (int*) palloc((numLeaves) * sizeof(int));
	subtrees = (treeNode**) palloc((numLeaves) * sizeof(treeNode*));
	/*
	 * Initializing values...
	 */
	numSubtrees = numLeaves;
	for (i=0; i < numLeaves; i++)
	{
		parent[i] = i;       // todos os vÃ©rtices sÃ£o raÃ­zes de Ã¡rvores
		weight[i] = 1;       // todas as Ã¡rvores tÃªm 1 vÃ©rtice
		subtrees[i] = NULL;
	}
	/*
	 * For each edge or while exists more that 1 sub-tree.
	 * Verify whether the edge belong to minimal spanning tree.
	 */
	for (i=0; i < numEdges && numSubtrees > 1; i++) // edge-by-edge loop
	{
		int root1, root2;

		/*
		 * Test the root of each relation in selected edge.
		 */
		root1 = find_root(edgeList[i]->head_idx, parent);
		root2 = find_root(edgeList[i]->tail_idx, parent);
		/*
		 * If both roots is not the same, the edge belong to the minimal
		 * spanning tree. Join the trees in parent[] and the execution plan
		 * in subplan_list[].
		 */
		if (root1 != root2)
		{
			int other_root;

			/*
			 * Join two trees. root1 is the root of new tree.
			 */
			join_trees(&root1, &root2, weight, parent, &numSubtrees);
			last_join = root1;
			other_root = root2;

			/*
			 * Juntando planos de execuÃ§Ã£o:
			 */
			if( ! subtrees[last_join] ){
				/*
				 * First join of tree.
				 */
				subtrees[last_join] = edgeList[i];
			} else if( ! subtrees[other_root] ) {
				/*
				 * Left-deep join.
				 * Join one relation to a composed plan.
				 */
				treeNode *new_node;
				new_node = joinNodes( root, subtrees[last_join],
						leafNodes + other_root );
				if( new_node ){
					subtrees[last_join] = new_node;
				} else {
					elog(ERROR, "NÃ£o foi possÃ­vel fazer left-deep join.");
				}
			} else {
				/*
				 * Bushy-join.
				 * Join two composed plans.
				 */
				treeNode *new_node;
				new_node = joinNodes(root, subtrees[last_join],
				                           subtrees[other_root]);
				if( new_node ){
					subtrees[last_join] = new_node;
				} else {
					elog(ERROR, "NÃ£o foi possÃ­vel fazer bushy-join.");
				}
			}
		}
	}

	if( last_join == -1 ){ // exception
		elog(ERROR,"NÃ£o foi possÃ­vel gerar o plano.");
		return NULL;
	}

	return subtrees[last_join];
}

static void
randomState(treeNode **newState/*OUT*/,
            treeNode **stateList/*IN*/, int stateSize/*IN*/)
{
	int          i,j,
	             count = stateSize,
	             item;
	treeNode   **list;

	list = (treeNode**) palloc(stateSize * sizeof(treeNode*));
	for ( i=0; i<stateSize; i++ ){
		list[i] = stateList[i];
	}
	for ( i=0; i<stateSize; i++ ){
		item = random()%count--;
		newState[i] = list[item];
		for( j=item; j<count; j++ ){
			list[j] = list[j+1];
		}
	}
	pfree(list);
}

static tempCtx*
createTemporaryContext( PlannerInfo *root )
{
	tempCtx *ctx;
	ctx = (tempCtx*) palloc(sizeof(tempCtx));

	ctx->mycontext = AllocSetContextCreate(CurrentMemoryContext,
									  "TwoPO",
									  ALLOCSET_DEFAULT_MINSIZE,
									  ALLOCSET_DEFAULT_INITSIZE,
									  ALLOCSET_DEFAULT_MAXSIZE);
	ctx->oldcxt = MemoryContextSwitchTo(ctx->mycontext);
	ctx->savelength = list_length(root->join_rel_list);
	ctx->savehash = root->join_rel_hash;

	return ctx;
}

static void
restoreOldContext( PlannerInfo *root, tempCtx *ctx,
                   treeNode **edgeList, int numEdges )
{
	int i;

	root->join_rel_list = list_truncate(root->join_rel_list,
										  ctx->savelength);
	root->join_rel_hash = ctx->savehash;
	MemoryContextSwitchTo(ctx->oldcxt);
	MemoryContextDelete(ctx->mycontext);
	pfree(ctx);

	/*
	 * Cleaning parent nodes in edgeList deleted by MemoryContextDelete()
	 */
	for( i=0; i<numEdges; i++ ){
		edgeList[i]->parents = NULL;
	}
}

static void
neighbordState(treeNode** newState/*OUT*/,
               treeNode** state/*IN*/, int stateSize/*IN*/)
{
	int i;
	int item;
	int method;
	treeNode *aux;

	if( stateSize < 2 ) {
		elog( ERROR, "neighbordState(): stateSize invalid ( < 2 ).");
	} else if( stateSize == 2 ) {
		newState[1] = state[0];
		newState[0] = state[1];
	} else {
		for ( i=0; i<stateSize; i++ ){
			newState[i] = state[i] ;
		}

		item = random() % stateSize;
		method = random() % 2;

		switch( method ) {
		case 0:  // swap method
			aux = newState[item];
			newState[item] = newState[(item+1)%stateSize];
			newState[(item+1)%stateSize] = aux;
			break;
		default: // 3cycle method (simple)
			aux = newState[item];
			newState[item] = newState[(item+2)%stateSize];
			newState[(item+2)%stateSize] = newState[(item+1)%stateSize];
			newState[(item+1)%stateSize] = aux;
		}
	}
}

static Cost
stateCost(PlannerInfo *root, treeNode *leafNodes, int numLeaves,
          treeNode **state, int stateSize)
{
	treeNode *node;
	node = buildBushyTree( root, leafNodes, numLeaves,
			state, stateSize);
	return nodeCost(node);
}

static Cost
improveState(PlannerInfo *root/*IN*/,
             treeNode *leafNodes/*IN*/, int numLeaves/*IN*/,
             treeNode **currentState/*IN*/, int stateSize/*IN*/,
             treeNode **newState/*OUT*/)
{
	treeNode   **new_state;
	treeNode   **cheapest_state;
	Cost         new_cost;
	Cost         cheapest_cost;
	int          i;
	int          local_minimum = stateSize;

	new_state      = (treeNode**) palloc(stateSize * sizeof(treeNode*));
	cheapest_state = (treeNode**) palloc(stateSize * sizeof(treeNode*));

	for( i=0; i<stateSize; i++ )
		cheapest_state[i] = currentState[i];
	cheapest_cost = stateCost( root, leafNodes, numLeaves,
			cheapest_state, stateSize);

	i = 0;
	while( i < local_minimum ){
		neighbordState(new_state,cheapest_state,stateSize);
		new_cost = stateCost( root, leafNodes, numLeaves,
				new_state, stateSize);
		if( new_cost < cheapest_cost )
		{
			swapValues(treeNode**,cheapest_state,new_state);
			cheapest_cost = new_cost;
			i=0;
		} else {
			i++;
		}
	}

	for( i=0; i<stateSize; i++ ){
		newState[i] = cheapest_state[i];
	}
	pfree(new_state);
	pfree(cheapest_state);

	return cheapest_cost;
}

static void
prepareOptimizer( PlannerInfo *root, int levels_needed, List *initial_rels,
		treeNode **leafNodes/*OUT*/,
		treeNode ***edgeList/*OUT*/, int *numEdges/*OUT*/)
{
	ListCell    *x;
	int          i;
	List        *twoLevelNodesList;
	treeNode    *node;

	*leafNodes = buildOneLevelNodes(initial_rels,levels_needed);

	twoLevelNodesList = buildTwoLevelNodes(root, *leafNodes, levels_needed);
	if( !twoLevelNodesList ) {
		elog(ERROR, "prepareOptimizer(): No two-level joins found.");
	}

	*numEdges = list_length( twoLevelNodesList );

	*edgeList     = (treeNode**) palloc((*numEdges) * sizeof(treeNode*));
	i = 0;
	foreach( x, twoLevelNodesList ){
		node = lfirst(x);
		(*edgeList)[i++] = node;
	}
}

static int
initialStateHeuristic_1(const void* xin, const void* yin)
{
	treeNode **x,**y;
	Cost       cx, cy;

	x=(treeNode**) xin;
	y=(treeNode**) yin;
	cx=nodeCost((*x));
	cy=nodeCost((*y));

	if (cx > cy)
		return 1;
	else if (cx < cy)
		return -1;

	return 0;
}

static void
makeInitialState(treeNode **newState /*OUT*/,
		treeNode **edgeList/*IN*/, int numEdges/*IN*/, int iteratorIndex/*IN*/ )
{
	int i;

	if( twopo_heuristic_states && iteratorIndex == 0 ) { // initial state bias:
        //sort edges using heuristic 1
		for( i=0; i<numEdges; i++ )
			newState[i] = edgeList[i];
		qsort(newState,numEdges,sizeof(treeNode**),initialStateHeuristic_1);
	} else { // random states:
		randomState( newState, edgeList, numEdges );
	}
}

inline static bool
saProbability( Cost delta, double temperature )
{
	double e = exp( - delta / temperature );
	int r = random() % 100;
#	ifndef TWOPO_DEBUG
	return r <= ((int)(100.0*e));
#	else
	if ( r <= ((int)(100.0*e)) ) {
		fprintf(stderr, " sa_prob_ok" );
		return true;
	}
	return false;
#	endif
}

#ifdef TWOPO_DEBUG

static void
print_state(treeNode **edgeList, int numEdges,
		treeNode *leafNodes, int numLeaves)
{
	int i;
	for( i=0; i<numEdges; i++ ){
		fprintf(stderr, "(%d,%d) ",
				(int)(edgeList[i]->inner_child - leafNodes),
				(int)(edgeList[i]->outer_child - leafNodes));
	}
	fprintf(stderr,"\n");
}

static void
verificar(treeNode **state, treeNode **edgeList, int numEdges)
{
	int i,j;
	for( i=0; i<numEdges; i++ ){
		for( j=0; j<numEdges; j++ ){
			if( state[i] == edgeList[j] )
				break;
		}
		if( j >= numEdges )
			fprintf(stderr, " ERRO:edge_nÃ£o_encontrado");
	}
}

static void
verificar_iguais(treeNode **state, treeNode **edgeList, int numEdges)
{
	int i;
	for( i=0; i<numEdges; i++ ){
		if( state[i] != edgeList[i] )
			return;
	}
	fprintf(stderr, " ERRO:states_iguais");
}

#endif


RelOptInfo *
twopo(PlannerInfo *root, int levels_needed, List *initial_rels)
{
	tempCtx     *ctx;
	treeNode    *leafNodes;
	treeNode   **edgeList;
	int          numEdges;

	treeNode   **min_state;
	treeNode   **new_state;
	treeNode   **improved_state;
	Cost         min_cost = 0;
	Cost         improved_cost = 0;
	treeNode    *node;
	int          i;

	prepareOptimizer(root, levels_needed, initial_rels,
			&leafNodes, &edgeList, &numEdges);

	if( numEdges == 1 )
		return edgeList[0]->rel;

	min_state       = (treeNode**) palloc(numEdges * sizeof(treeNode*));
	new_state       = (treeNode**) palloc(numEdges * sizeof(treeNode*));
	improved_state  = (treeNode**) palloc(numEdges * sizeof(treeNode*));

	///////////////// Temporary memory context area ////////////////////////
	ctx = createTemporaryContext( root );

	////////////// II phase //////////////
	for( i=0; i<twopo_ii_stop; i++ ){
		makeInitialState( new_state, edgeList, numEdges, i );

		improved_cost = improveState( root, leafNodes, levels_needed,
				new_state, numEdges, improved_state );
		if( !i || min_cost > improved_cost ) {
			swapValues( treeNode**, min_state, improved_state );
			min_cost = improved_cost;
		}
	}
	////////////// SA phase ///////////////
	if( twopo_sa_phase ) {
		double  temperature = twopo_sa_initial_temperature * (double) min_cost;
		int     equilibrium = twopo_sa_equilibrium * numEdges;
		int     stage_count = 0;
		Cost    new_cost = 0;
		Cost    delta_cost;

#		ifdef TWOPO_DEBUG
		fprintf(stderr, " min_cost:%.1lf", min_cost);
#		endif

		improved_cost = min_cost;
		for( i=0; i<numEdges; i++ ){ // setting S state
			improved_state[i] = min_state[i];
		}
		while( temperature >= 1 && stage_count < 5 ){ // frozen condition

			for( i=0; i<equilibrium; i++ ){
				neighbordState( new_state, improved_state, numEdges );
				new_cost = stateCost( root, leafNodes, levels_needed,
						new_state, numEdges );
				delta_cost = new_cost - improved_cost;

				if( delta_cost <= 0 || saProbability(delta_cost,temperature) ){
#					ifdef TWOPO_DEBUG
					verificar(new_state,edgeList,numEdges);
					verificar_iguais(new_state,improved_state,numEdges);
#                   endif

					swapValues(treeNode**,new_state,improved_state);
					improved_cost = new_cost;

					if( improved_cost < min_cost ){
						int j;
						for( j=0; j<numEdges; j++ ) { // update min_state
							min_state[j] = improved_state[j];
						}
						min_cost = improved_cost;
						stage_count = 0;
#						ifdef TWOPO_DEBUG
						fprintf(stderr, " sa_new_min_cost:%.2lf", min_cost);
#						endif
					}
				}
			}

			stage_count++;
			temperature *= twopo_sa_temperature_reduction; //reducing temperature
		}
	}


	restoreOldContext( root, ctx, edgeList, numEdges );
	//////////////// end of temporary memory context area //////////////////

	// rebuild best state in correct memory context
	node = buildBushyTree( root, leafNodes, levels_needed,
			min_state, numEdges);

	pfree(min_state);
	pfree(new_state);
	pfree(improved_state);

	return node->rel;
}
