#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#include "defs.h"

/* DisjointSet data structure http://en.wikipedia.org/wiki/Disjoint-set_data_structure */
typedef struct {
    vertex_id_t parent;
    int rank;
} DisjointSetElement;

typedef DisjointSetElement *DisjointSet;

/* Initialize disjoint set, call once to allocate memory */
void UF_Init(DisjointSet* set, int maxElements)
{
    *set = malloc(sizeof(DisjointSetElement) * maxElements);
}

/* Put given vertex into a separate set */
void MakeSet(DisjointSet set, vertex_id_t vertex)
{
    set[vertex].parent = vertex;
    set[vertex].rank = 0;
}

/* Find the representative vertex */
vertex_id_t Find(DisjointSet set, vertex_id_t vertex)
{
    if (set[vertex].parent != vertex) {
        set[vertex].parent = Find(set, set[vertex].parent);
    }

    return set[vertex].parent;
}

/* Create a union of two sets containing x and y */
vertex_id_t Union(DisjointSet set, vertex_id_t x, vertex_id_t y)
{
    const vertex_id_t xRoot = Find(set, x);
    const vertex_id_t yRoot = Find(set, y);

    if (xRoot == yRoot) {
        return xRoot;
    }

    if (set[xRoot].rank < set[yRoot].rank) {
        set[xRoot].parent = yRoot;
        return yRoot;
    }
    
    if (set[xRoot].rank > set[yRoot].rank) {
        set[yRoot].parent = xRoot;
        return xRoot;
    }

    set[yRoot].parent = xRoot;
    set[xRoot].rank++;
    return xRoot;
}

/* Linked list of edges to store edges of the spanning tree */
typedef struct tagEdgeList {
    edge_id_t edge;
    struct tagEdgeList* next;
} EdgeElement;

typedef struct {
    EdgeElement* first;
    EdgeElement* last;
} EdgeList;

EdgeList empty_list = {0, 0};

/* Add a new edge to the list */
void list_push(EdgeList* list, edge_id_t edge)
{
    EdgeElement* new = (EdgeElement*)malloc(sizeof(EdgeElement));

    new->edge = edge;
    new->next = list->first;

    list->first = new;

    if( list->last == 0 ) {
        list->last = list->first;
    }
}

/* Unite two lists */
void list_unite(EdgeList *dest, EdgeList src)
{
    if( dest->first == 0 ) {
        *dest = src;
    } else if( src.first == 0 ) {
        // do nothing
    } else {
        dest->last->next = src.first;
        dest->last = src.last;
    }
}

typedef struct {
    vertex_id_t numTrees;
    /* Array of edge lists: each edge list is attached to the representative vertex in the disjoint set */
    EdgeList *trees;
} Result;

Result result;

/* Array for finding shortest edges between disjoint sets */
edge_id_t *shortest_edge;

/* Disjoint set data structure instance */
DisjointSet set;

void init_mst(graph_t *G)
{
    UF_Init(&set, G->n);
    shortest_edge = (edge_id_t*)malloc(G->n * sizeof(edge_id_t));

    result.trees = (EdgeList*)malloc(G->n * sizeof(EdgeList));
}

/* Boruvka's algorithm (http://en.wikipedia.org/wiki/Bor%C5%AFvka%27s_algorithm) implementation */
void* MST(graph_t *G)
{
    /* Initially each vertex is put into a set on its own */
    for( vertex_id_t v = 0; v < G->n; v++ ) {
        MakeSet(set, v);
    }

    /* Count number of trees in the resulting forest by decreasing G->n by 1 for each edge added to the forest
     * Becase for a forest N = M - E holds where N -- number of vertices, M -- number of trees, E -- number of edges in the trees */
    result.numTrees = G->n;

    /* Initialize edge lists to empty ones */
    for( vertex_id_t v = 0; v < G->n; v++ ) {
        result.trees[v] = empty_list;
    }

    /* We iterate until we were unable to unite any two disjoint sets */
    bool united = true;

    while( united ) {
        united = false;

        /* -1 stands for "infinity": initial value for searching for the shortest edge between pairs of disjoint sets */
        for( vertex_id_t v = 0; v < G->n; v++ ) {
            shortest_edge[v] = -1;
        }

        /* Iterate over all edges */
        for( vertex_id_t v = 0; v < G->n; v++ ) {
            /* Basic compressed row storage iteration */
            for( edge_id_t e = G->rowsIndices[v]; e < G->rowsIndices[v + 1]; e++ ) {
                const vertex_id_t u = G->endV[e];
                const vertex_id_t vRep = Find(set, v);
                const vertex_id_t uRep = Find(set, u);

                if( vRep != uRep ) { // Only test the edges from different sets: representatives are not equal
                    if( shortest_edge[vRep] == -1 || G->weights[e] < G->weights[shortest_edge[vRep]] ) {
                        /* We store minimum in the representative vertex for the set */
                        shortest_edge[vRep] = e;
                    }
                }
            }
        }

        /* Add shortest edges to the forest */
        for( vertex_id_t v = 0; v < G->n; v++ ) {
            const edge_id_t e = shortest_edge[v];
            if( e != -1 ) {
                const vertex_id_t u = G->endV[e];
                const vertex_id_t vRep = Find(set, v);
                const vertex_id_t uRep = Find(set, u);

                if( vRep != uRep ) {
                    /* Union happened, another iteration is needed */
                    united = true;
                    result.numTrees--;
                    const vertex_id_t new = Union(set, vRep, uRep); // new is the new representative of the united set

                    /* Now unite edge lists stored in old sets */
                    EdgeList tree = result.trees[vRep];
                    list_unite(&tree, result.trees[uRep]);
                    
                    /* Add newly added edge to the edge list */
                    list_push(&tree, e);

                    /* Move edge list to the new representative vertex of the united set */
                    result.trees[vRep] = result.trees[uRep] = empty_list;
                    result.trees[new] = tree;
                }
            }
        }
    }

    return &result;
}

/* Convert Result structure to forest_t for validation */
void convert_to_output(graph_t *G, void* presult, forest_t *trees_output)
{
    Result* result = (Result*) presult;
    trees_output->numTrees = result->numTrees;
    trees_output->numEdges = G->n - result->numTrees;
    trees_output->p_edge_list = (edge_id_t*)malloc(2 * result->numTrees * sizeof(edge_id_t));

    for( int i = 0; i < result->numTrees * 2; i++ ) {
        trees_output->p_edge_list[i] = 0;
    }
    trees_output->edge_id = (edge_id_t*)malloc(trees_output->numEdges * sizeof(edge_id_t));

    int tree_index = 0;
    edge_id_t edge_index = 0;

    for( vertex_id_t v = 0; v < G->n; v++ ) {
        if( result->trees[v].first != 0 ) {
            trees_output->p_edge_list[tree_index] = edge_index;
            EdgeElement* p = result->trees[v].first;
            while( p != 0 ) {
                trees_output->edge_id[edge_index++] = p->edge;
                EdgeElement* old = p;
                p = p->next;
                free(old);
            }
            tree_index++;
            trees_output->p_edge_list[tree_index] = edge_index;
            tree_index++;
        }
    }
}

void finalize_mst(graph_t *G)
{
    free(shortest_edge);
    free(result.trees);
    free(set);
}
