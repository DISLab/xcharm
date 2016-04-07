#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <assert.h>
#include <math.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include <string.h>
#include "defs.h"

void write_output_information(forest_t *trees, char *filename)
{
    FILE *F = fopen(filename, "wb");
    assert(fwrite(&trees->numTrees, sizeof(vertex_id_t), 1, F) == 1);
    assert(fwrite(&trees->numEdges, sizeof(edge_id_t), 1, F) == 1);
    assert(fwrite(trees->p_edge_list, sizeof(edge_id_t), 2*trees->numTrees, F) == 2*trees->numTrees);
    assert(fwrite(trees->edge_id, sizeof(edge_id_t), trees->numEdges, F) == trees->numEdges);
    fclose(F);
}

void readGraph(graph_t *G, char *filename)
{
    uint8_t align;
    FILE *F = fopen(filename, "rb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);
    
    assert(fread(&G->n, sizeof(vertex_id_t), 1, F) == 1);
    G->scale = log(G->n) / log (2);
    
    assert(fread(&G->m, sizeof(edge_id_t), 1, F) == 1);
    assert(fread(&G->directed, sizeof(bool), 1, F) == 1);
    assert(fread(&align, sizeof(uint8_t), 1, F) == 1);
    
    G->rowsIndices = (edge_id_t *)malloc((G->n+1) * sizeof(edge_id_t));
    assert(G->rowsIndices); 
    assert(fread(G->rowsIndices, sizeof(edge_id_t), G->n+1, F) == (G->n+1));
    G->endV = (vertex_id_t *)malloc(G->rowsIndices[G->n] * sizeof(vertex_id_t));
    assert(G->endV);
    assert(fread(G->endV, sizeof(vertex_id_t), G->rowsIndices[G->n], F) == G->rowsIndices[G->n]);
    G->weights = (weight_t *)malloc(G->m * sizeof(weight_t));
    assert(G->weights);

    assert(fread(G->weights, sizeof(weight_t), G->m, F) == G->m);
    fclose(F);
}

void writeBinaryGraph(graph_t *G, char *filename)
{
    FILE *F = fopen(filename, "wb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);
    assert(fwrite(&G->n, sizeof(vertex_id_t), 1, F) == 1);
    assert(fwrite(&G->m, sizeof(edge_id_t), 1, F) == 1);
    assert(fwrite(&G->directed, sizeof(bool), 1, F) == 1);
    uint8_t align = 0;
    assert(fwrite(&align, sizeof(uint8_t), 1, F) == 1);
    assert(fwrite(G->rowsIndices, sizeof(edge_id_t), G->n+1, F) == G->n+1);
    assert(fwrite(G->endV, sizeof(vertex_id_t), G->rowsIndices[G->n], F) == G->rowsIndices[G->n]);
    assert(fwrite(G->weights, sizeof(weight_t), G->m, F) == G->m);
    fclose(F);
}

void writeTextGraph(graph_t *G)
{
    char filename[FNAME_LEN];
    if (strlen(G->filename) == 0) {
        sprintf(filename, "graph");
    } else {
        sprintf(filename, "%s", G->filename);
    }
    printf("%d:start write to file = %s\n",G->rank, filename);
    
    FILE *F = fopen(filename, "w");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);

    for (vertex_id_t i = 0; i < G->n; i++) {
        fprintf(F, " %" PRIu32 ": [",  i);
        for (edge_id_t j = G->rowsIndices[i]; j < G->rowsIndices[i+1]; j++) {
            fprintf(F, "[%d,%.3f]", G->endV[j], G->weights[j]);
            if (j != G->rowsIndices[i+1] - 1) fprintf(F, ",");
        }
        fprintf(F, "]\n");
    }
    fclose(F);
}

void freeGraph(graph_t *G)
{
    free(G->rowsIndices);
    free(G->endV);
    free(G->weights);
}


#ifdef USE_MPI
/*TODO*/
//void readGraph_rankFiles_MPI(graph_t *G, char *filename){}

void readGraph_singleFile_MPI(graph_t *G, char *filename)
{
    uint8_t align;
    int rank, size;
    int offset,offset_row ,offset_col,offset_weight;
    edge_id_t my_edges[2];
    int local_n=0;
    int local_m=0;
    int k;
    uint32_t TotVertices;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    G->rank = rank;
    G->nproc = size;
    G->filename[0] = '\0';
    sprintf(G->filename, "%s", filename);
    
    int lgsize; 
    for (lgsize = 0; lgsize < size; ++lgsize) {
        if ((1 << lgsize) == size) break;
    }

    MPI_File fh;
    MPI_Status status;    
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    MPI_File_read(fh, &G->n, 1, MPI_UINT32_T, &status);
    offset = sizeof(vertex_id_t);
    TotVertices = G->n;

    MPI_File_read_at(fh, offset, &G->m, 1, MPI_UINT64_T, &status);
    offset += sizeof(edge_id_t);
    
    MPI_File_read_at(fh, offset, &G->directed, 1, MPI_C_BOOL, &status);
    offset += sizeof(bool);

    MPI_File_read_at(fh, offset, &align, 1, MPI_UINT8_T, &status);
    offset += sizeof(uint8_t);
    offset_row = offset;

    for( uint32_t i = 0; i < G->n; i++ ) {
        if( rank == VERTEX_OWNER(i,TotVertices,size) ) {
            MPI_File_read_at(fh,offset_row + (i)*sizeof(edge_id_t),&my_edges[0], 2, MPI_UINT64_T, &status);
            local_n++;
            local_m += my_edges[1] - my_edges[0];
        }
    }
    G->local_n = local_n;
    G->local_m = local_m;
    offset_col = offset_row + (G->n+1) * sizeof(edge_id_t);
    offset_weight = offset_col + G->m * sizeof(vertex_id_t);

    G->rowsIndices = (edge_id_t *)malloc((G->local_n + 1) * sizeof(edge_id_t) );
    G->endV = (vertex_id_t *)malloc((G->local_m)*sizeof(vertex_id_t));
    G->weights = (weight_t *)malloc((G->local_m)*sizeof(weight_t));
    G->rowsIndices[0] = 0;
    k = 1;

    for( uint32_t i = 0; i < G->n; i++ ) {
        if( rank == VERTEX_OWNER(i,TotVertices,size) ) {
            MPI_File_read_at(fh,offset_row + (i)*sizeof(edge_id_t),&my_edges[0], 2, MPI_UINT64_T, &status);
            G->rowsIndices[k] = G->rowsIndices[k-1] + my_edges[1] - my_edges[0];
            MPI_File_read_at(fh,offset_col + my_edges[0] * sizeof(vertex_id_t), &G->endV[G->rowsIndices[k-1]], G->rowsIndices[k]-G->rowsIndices[k-1], MPI_UINT32_T, &status);
            MPI_File_read_at(fh,offset_weight + my_edges[0] * sizeof(weight_t), &G->weights[G->rowsIndices[k-1]], G->rowsIndices[k]-G->rowsIndices[k-1], MPI_DOUBLE, &status);
            k++;
        }
    }

    MPI_File_close(&fh);
}


void writeTextGraph_MPI(graph_t *G)
{
    uint32_t TotVertices;
    TotVertices=G->n;
    int size = G->nproc;
    int rank = G->rank;
    int lgsize;
    char filename[FNAME_LEN];
    for (lgsize = 0; lgsize < size; ++lgsize) {
        if ((1 << lgsize) == size) break;
    }
    if (strlen(G->filename) == 0) {
        sprintf(filename, "graph.%d", G->rank);
    } else {
        sprintf(filename, "%s.%d", G->filename, G->rank);
    }
    printf("%d:start write to file = %s\n",G->rank,filename);
    
    FILE *F = fopen(filename, "w");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);

    for (vertex_id_t i = 0; i < G->local_n; i++) {
        fprintf(F, " %" PRIu32 ": [",  VERTEX_TO_GLOBAL(i,TotVertices,size,rank));
        for (edge_id_t j = G->rowsIndices[i]; j < G->rowsIndices[i+1]; j++) {
            fprintf(F, "[%d,%f]", G->endV[j], G->weights[j]);
            if (j != G->rowsIndices[i+1] - 1) fprintf(F, ",");
        }
        fprintf(F, "]\n");
    }
    fclose(F);
}

void writeBinaryGraph_MPI(graph_t *G)
{
    char filename[FNAME_LEN];
    if (strlen(G->filename) == 0) {
        sprintf(filename, "graph.%d", G->rank);
    } else {
        sprintf(filename, "%s.%d", G->filename, G->rank);
    }
    FILE *F = fopen(filename, "wb");                                                
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);           
                                                                                    
    assert(fwrite(&G->n, sizeof(vertex_id_t), 1, F) == 1);                          
                                                                                    
    assert(fwrite(&G->m, sizeof(edge_id_t), 1, F) == 1);                            
    assert(fwrite(&G->local_n, sizeof(vertex_id_t), 1, F) == 1);                    
    assert(fwrite(&G->local_m, sizeof(edge_id_t), 1, F) == 1);                      
    assert(fwrite(&G->nproc, sizeof(int), 1, F) == 1);                              
    assert(fwrite(&G->directed, sizeof(bool), 1, F) == 1);                          
    uint8_t align = 0;                                                              
    assert(fwrite(&align, sizeof(uint8_t), 1, F) == 1);                             
assert(fwrite(G->rowsIndices, sizeof(edge_id_t), G->local_n+1, F) == G->local_n + 1);                                                                                 
    assert(fwrite(G->endV, sizeof(vertex_id_t), G->rowsIndices[G->local_n], F) == G->rowsIndices[G->local_n]);                                                          
    assert(fwrite(G->weights, sizeof(weight_t), G->rowsIndices[G->local_n], F) == G->rowsIndices[G->local_n]);                                                          
    fclose(F);                                                                      
}
#endif
