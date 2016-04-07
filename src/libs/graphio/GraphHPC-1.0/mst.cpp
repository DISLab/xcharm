#include "defs.h"
using namespace std;

#ifdef USE_MPI
#include <mpi.h>

int rank,size,lgsize;
uint32_t TotVertices;

/* Write your own distributed MST implementation */

extern "C" void init_mst(graph_t *G)
{   
    TotVertices = G->n;
    rank = G->rank;
    size = G->nproc;
    for (lgsize = 0; lgsize < size; ++lgsize) {
        if ((1 << lgsize) == size) break;
    }
}   

extern "C" void* MST(graph_t *G)
{
    return 0;
}

extern "C" void convert_to_output(graph_t *G, void* result, forest_t *trees_output)
{
}

extern "C" void finalize_mst()
{
}
#else 

/* Write your own shared-memory MST implementation */

extern "C" void init_mst(graph_t *G)
{   
}   

extern "C" void* MST(graph_t *G)
{
    return 0;
}

extern "C" void convert_to_output(graph_t *G, void* result, forest_t *trees_output)
{
}

extern "C" void finalize_mst(graph_t *G)
{
}

#endif
