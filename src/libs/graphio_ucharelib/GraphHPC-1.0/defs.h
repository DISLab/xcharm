#ifndef __GRAPH_HPC_DEFS_H
#define __GRAPH_HPC_DEFS_H
#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define DEFAULT_ARITY 16
#define SMALL_COMPONENT_EDGES_THRESHOLD   2
#define FNAME_LEN   256
#define WEIGHT_ERROR 0.0001

typedef uint32_t vertex_id_t;
typedef uint64_t edge_id_t;
typedef double weight_t;

/* The graph data structure*/
typedef struct
{
    /***
     The minimal graph repesentation consists of:
     n        -- the number of vertices
     m        -- the number of edges
     endV     -- an array of size m that stores the 
                 destination ID of an edge <src->dest>.
     rowsIndices -- an array of size n+1 that stores the degree 
                 (out-degree in case of directed graphs) and pointers to
                 the endV array. The degree of vertex i is given by 
                 rowsIndices[i+1]-rowsIndices[i], and the edges out of i are
                 stored in the contiguous block endV[rowsIndices[i] .. rowsIndices[i+1]-1].
     Vertices are ordered from 0 in our internal representation
     ***/
    vertex_id_t n;
    edge_id_t m;
    edge_id_t* rowsIndices;
    vertex_id_t* endV;

    /* Edge weights */
    weight_t* weights;
    weight_t min_weight, max_weight;

    /* other graph parameters */
    int scale; /* log2 of vertices number */
    int avg_vertex_degree; /* relation m / n */
    bool directed; 

    /* RMAT graph parameters */
    double a, b, c;     
    bool permute_vertices;
    
    /* Distributed version variables */
    int nproc, rank;
    vertex_id_t local_n; /* local vertices number */
    edge_id_t local_m; /* local edges number */
    edge_id_t* num_edges_of_any_process;

    char filename[FNAME_LEN]; /* filename for output graph */
} graph_t;

typedef struct
{
    vertex_id_t numTrees;
    edge_id_t numEdges;
    edge_id_t* p_edge_list;
    edge_id_t* edge_id;

} forest_t;


/* write graph to file */
void writeGraph(graph_t *G, char *filename);
void writeBinaryGraph(graph_t *G, char *filename);
void writeTextGraph(graph_t *G);
void writeTextGraph_MPI(graph_t *G);
void writeBinaryGraph_MPI(graph_t *G);

/* read graph from file */
void readGraph(graph_t *G, char *filename);
void readGraph_rankFiles_MPI(graph_t *G, char *filename);
void readGraph_singleFile_MPI(graph_t *G, char *filename);

/* free graph memory */
void freeGraph(graph_t *G);

void write_output_information(forest_t *trees, char *filename);

#ifdef __cplusplus
    #define EXTERN_DECL extern "C"
#else
    #define EXTERN_DECL 
#endif

/* Minimum spanning tree */
EXTERN_DECL void* MST (graph_t *G);
EXTERN_DECL void convert_to_output(graph_t *G, void *result, forest_t* output); 

/* initialize algorithm memory */
EXTERN_DECL void init_mst(graph_t *G);
EXTERN_DECL void finalize_mst(graph_t *G);
EXTERN_DECL void gen_SSCA2_graph_MPI(graph_t *G);
EXTERN_DECL void gen_RMAT_graph_MPI(graph_t *G);

/* returns global number of the edge from local number */
edge_id_t edge_to_global(edge_id_t, graph_t*);

#define MOD_SIZE(v) ((v) % size)
#define DIV_SIZE(v) ((v) / size)
#define MUL_SIZE(x) ((x) * size)

/* returns number of vertex owner, v - the global vertex number, TotVertices - the global number of vertices, size - the number of processes*/
inline int VERTEX_OWNER(const vertex_id_t v, const vertex_id_t TotVertices, const int size)
{
    vertex_id_t mod_size = MOD_SIZE(TotVertices);
    vertex_id_t div_size = DIV_SIZE(TotVertices);
    if (!mod_size)
        return v / div_size;
    else
    {
        if (v / (div_size + 1) < mod_size)
            return v / (div_size + 1);
        else
            return (v - mod_size * (div_size + 1)) / div_size + mod_size;
    }
}

/* returns local vertex number, v - the global vertex number, TotVertices - the global number of vertices, size - the number of processes, rank - the process number*/
inline vertex_id_t VERTEX_LOCAL(const vertex_id_t v, const vertex_id_t TotVertices, const int size, const int rank) 
{
    if (MOD_SIZE(TotVertices) <= (unsigned int) rank)
        return ((v - MOD_SIZE(TotVertices) * (DIV_SIZE(TotVertices) + 1))%DIV_SIZE(TotVertices));
    else
        return (v%(DIV_SIZE(TotVertices) + 1));
}

/* returns global vertex number, v_local - the local vertex number, TotVertices - the global number of vertices, size - the number of processes, rank - the process number*/
inline vertex_id_t VERTEX_TO_GLOBAL(const vertex_id_t v_local, const vertex_id_t TotVertices, const int size, const int rank)
{
    if(MOD_SIZE(TotVertices) > (unsigned int) rank )
        return ((DIV_SIZE(TotVertices) + 1)*rank + (vertex_id_t) v_local);
    else
        return (MOD_SIZE(TotVertices)*(DIV_SIZE(TotVertices) + 1) + DIV_SIZE(TotVertices)*(rank - MOD_SIZE(TotVertices)) + v_local);
}

#endif
