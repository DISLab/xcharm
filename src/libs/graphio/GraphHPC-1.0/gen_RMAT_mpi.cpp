#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <error.h>
#include <mpi.h>
#include <math.h>
#include <limits.h>
#include "defs.h"

bool writeTextFile = false;

/* function returns size of edges block for current process */
edge_id_t get_local_m(graph_t *G)
{
    edge_id_t m = G->m;
    edge_id_t local_m;
    int nproc = G->nproc;
    int rank = G->rank;
    if (rank == nproc - 1) local_m = m/nproc + m % nproc;
    else local_m = m/nproc;
    return local_m;
}

// scrambling function similar to Cray XMT
vertex_id_t scrambling_vertex(vertex_id_t num_vertex)
{
    uint32_t scrambleMatrix[10] = {
        0x58022200,// 01011000000000100010001000000000 
        0xC812A300,// 11001000000100101010001100000000 
        0xEA73AB80,// 11101010011100111010101110000000
        0x49C966C0,// 01001001110010010110011011000000
        0x324FFC60,// 00110010010011111111110001100000
        0x1A8AD370,// 00011010100010101101001101110000
        0xA53F48A8,// 10100101001111110100100010101000
        0xD8256A0C,// 11011000001001010110101000001100
        0x2779B842,// 00100111011110011011100001000010 
        0x388647B1 // 00111000100001100100011110110001
    };
    
    uint32_t res_10bit = 0;
    for (int i = 0; i < 10; ++i) {
        uint32_t v = num_vertex;
        v &= scrambleMatrix[i];
        uint32_t tmp = 0;
        for (int j = 0; j < 32; ++j) {
            tmp = (v & 1) ^ tmp; 
            v = v >> 1; 
        }
        res_10bit = res_10bit | (tmp << (9-i));
    }
    num_vertex = ((num_vertex >> 10) << 10) + res_10bit;
    return num_vertex;
}

/* function is adapted from SNAP
 * http://snap-graph.sourceforge.net/ */
void gen_RMAT_graph_MPI(graph_t* G) 
{
    bool undirected;
    vertex_id_t n, local_n;
    edge_id_t  local_m, num_dir_edges;
    edge_id_t offset;
    double a, b, c, d;
    double av, bv, cv, dv, S, p;
    int SCALE;
    double var;
    vertex_id_t step;
    weight_t *weight;
    weight_t min_weight, max_weight;
    bool permute_vertices;
    vertex_id_t u, v;
    int seed;
    vertex_id_t *edges;
    vertex_id_t *degree;
    
    int size, rank, lgsize;
    
    G->directed = false;
    G->a = 0.45;
    G->b = 0.25;
    G->c = 0.15;
    G->permute_vertices = true;
    if (sizeof(vertex_id_t) == sizeof(uint64_t) && G->permute_vertices) {
        if (G->rank == 0) printf("permutation of the vertices are not supported for uint64_t \n");
        MPI_Finalize();
        exit(1);
    }
    G->min_weight = 0;
    G->max_weight = 1;
    G->avg_vertex_degree = DEFAULT_ARITY;
    G->filename[0] = '\0';
    G->n = (vertex_id_t)1 << G->scale;
    G->m = G->n * (edge_id_t)G->avg_vertex_degree;
    if (strlen(G->filename) == 0) sprintf(G->filename, "rmat-%d", G->scale);
    

    a = G->a;
    b = G->b;
    c = G->c;
    assert(a+b+c < 1);
    d = 1  - (a+b+c);

    permute_vertices = G->permute_vertices;

    undirected = !G->directed;
    n = G->n;
    uint32_t TotVertices;
    TotVertices = G->n;

    MPI_Datatype MPI_VERTEX_ID_T;
    MPI_Type_contiguous(sizeof(vertex_id_t), MPI_BYTE, &MPI_VERTEX_ID_T);
    MPI_Type_commit(&MPI_VERTEX_ID_T);

    rank = G->rank;
    size = G->nproc;
    for (lgsize = 0; lgsize < size; ++lgsize) {
        if ((1 << lgsize) == size) break;
    }
    /* get size of vertices and edges blocks for current processes */
    local_m = get_local_m(G);
    local_n = n/size;
    num_dir_edges = local_m;
    if (undirected) local_m = 2*local_m;
    G->local_n = local_n;
    G->local_m = local_m;
    edges = (vertex_id_t *) malloc (2 /* one edge consists of two vertex */ *num_dir_edges * sizeof(vertex_id_t)); 
    degree = (vertex_id_t *) calloc(local_n, sizeof(vertex_id_t));
    assert(edges != NULL);
    assert(degree != NULL);

    weight = (weight_t *) malloc(num_dir_edges * sizeof(weight_t));
    assert(weight != NULL);

    /* Initialize RNG stream */
    seed = 2387 + rank;
    srand48(seed);

    SCALE = G->scale;
    /* Generate edges */
    for (edge_id_t i=0; i<num_dir_edges; i++) {

        u = 1;
        v = 1;
        step = n/2;

        av = a;
        bv = b;
        cv = c;
        dv = d;
        p = drand48();
        if (p < av) {
            /* Do nothing */
        } else if ((p >= av) && (p < av+bv)) {
            v += step;
        } else if ((p >= av+bv) && (p < av+bv+cv)) {
            u += step;
        } else {
            u += step;
            v += step;
        }

        for (int j=1; j<SCALE; j++) {
            step = step/2;

            /* Vary a,b,c,d by up to 10% */
            var = 0.1;
            av *= 0.95 + var * drand48();
            bv *= 0.95 + var * drand48();
            cv *= 0.95 + var * drand48();
            dv *= 0.95 + var * drand48();

            S = av + bv + cv + dv;
            av = av/S;
            bv = bv/S;
            cv = cv/S;
            dv = dv/S;

            /* Choose partition */
            p = drand48();
            if (p < av) {
                /* Do nothing */
            } else if ((p >= av) && (p < av+bv)) {
                v += step;
            } else if ((p >= av+bv) && (p < av+bv+cv)) {
                u += step;
            } else {
                u += step;
                v += step;
            }
        }
        if (permute_vertices) {
            edges[2*i+0] = scrambling_vertex(u-1);
            edges[2*i+1] = scrambling_vertex(v-1);
        }
        else {
            edges[2*i+0] = u-1;
            edges[2*i+1] = v-1;
        }
    }

    min_weight = G->min_weight;
    max_weight = G->max_weight;
    /* Generate edges weights */
    for (edge_id_t i=0; i<num_dir_edges; i++) {
        weight[i]  = min_weight + (max_weight-min_weight)*drand48();
    }
    weight_t *send_weight = (weight_t*) malloc (local_m * sizeof(weight_t));
    assert(send_weight != NULL);
    weight_t *recv_weight;
    vertex_id_t* send_edges = (vertex_id_t*) malloc (2 * local_m * sizeof(vertex_id_t));
    assert(send_edges != NULL);
    vertex_id_t* recv_edges;
    int* send_counts = (int*) calloc (size, sizeof(int));
    assert(send_counts != NULL);
    int* recv_counts = (int*) calloc (size, sizeof(int));
    assert(recv_counts != NULL);
    edge_id_t* send_offsets_edge = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(send_offsets_edge != NULL);
    edge_id_t* recv_offsets_edge = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(recv_offsets_edge != NULL);
    edge_id_t* send_offsets_weight = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(send_offsets_weight != NULL);
    edge_id_t* recv_offsets_weight = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(recv_offsets_weight != NULL);
    /* calc count of data in each process */
    for (edge_id_t i=0; i<num_dir_edges; i++) {
        int proc_id = VERTEX_OWNER(edges[2*i+0], TotVertices, size);
        if (send_counts[proc_id] == INT_MAX / 2) {
            printf("send_counts (number of edges for sending ) > MAX_INT \n");
            MPI_Finalize();
            exit(1);
        }
	    send_counts[proc_id]++;
        if (undirected) {
            int proc_id = VERTEX_OWNER(edges[2*i+1], TotVertices, size);
            if (send_counts[proc_id] == INT_MAX / 2) {
                printf("send_counts (number of edges for sending ) > MAX_INT \n");
                MPI_Finalize();
                exit(1);
            }
            send_counts[proc_id]++;
        }
    }
    /* calc offsets */
    for (int i=1; i<size; i++) {
	    send_offsets_edge[i] = send_offsets_edge[i-1] + 2 * send_counts[i-1];
        send_offsets_weight[i] = send_offsets_weight[i-1] + send_counts[i-1];
    }
    /* clear send_counts for next using */
    for (int i=0; i<size; i++) {
	    send_counts[i] = 0;
    }
    /* copy edges and weight to send_data */
    for (edge_id_t i=0; i<num_dir_edges; i++) {
	    int proc_id = VERTEX_OWNER(edges[2*i+0], TotVertices, size);
        offset = send_offsets_edge[proc_id] + 2 * send_counts[proc_id];
	    send_edges[offset + 0] = VERTEX_LOCAL(edges[2*i+0], TotVertices, size, rank);
	    send_edges[offset + 1] = edges[2*i+1];
        offset = send_offsets_weight[proc_id] + send_counts[proc_id];
        send_weight[offset] = weight[i];
        send_counts[proc_id]++;
        if (undirected) {
            int proc_id = VERTEX_OWNER(edges[2*i+1], TotVertices, size);
            offset = send_offsets_edge[proc_id] + 2 * send_counts[proc_id];
            send_edges[offset + 0] = VERTEX_LOCAL(edges[2*i+1], TotVertices, size, rank);
            send_edges[offset + 1] = edges[2*i+0];
            offset = send_offsets_weight[proc_id] + send_counts[proc_id];
            send_weight[offset] = weight[i];
            send_counts[proc_id]++;        
        }
    }
    free(edges);
    free(weight);
    MPI_Request request[size];
    MPI_Status status[size];
    /* report counts to each process*/
    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);

    /* calc offsets and number of elements for MPI_Send */
    for (int i=1; i<size; i++) {
        recv_offsets_weight[i] = recv_offsets_weight[i-1] + recv_counts[i-1];
    }
    edge_id_t counts = 0;
    for(int i=0; i<size; i++) {
        counts += recv_counts[i];
    }
    recv_weight = (weight_t*) malloc (counts * sizeof(weight_t));
    assert(recv_weight != NULL);

    /* send weights to each process */ 
    for (int i = 0; i < size; i++) {
        MPI_Irecv(&recv_weight[recv_offsets_weight[i]], recv_counts[i], MPI_DOUBLE, i, G->rank, MPI_COMM_WORLD, &request[i]);
    }   
    for (int i = 0; i < size; i++) {
        MPI_Send(&send_weight[send_offsets_weight[i]], send_counts[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD);
    }
    MPI_Waitall(size, request, status);   

    free(send_weight);
    free(send_offsets_weight);
    free(recv_offsets_weight);
    
    /* report edges to each process */

    /* calc offsets and number of elements for the next MPI_Send */
    for (int i=0; i<size; i++) {
        recv_counts[i] = 2*recv_counts[i];
        send_counts[i] = 2*send_counts[i]; 
    }
    for (int i=1; i<size; i++) {
        recv_offsets_edge[i] = recv_offsets_edge[i-1] + recv_counts[i-1];
    }
    recv_edges = (vertex_id_t*) malloc (2*counts * sizeof(vertex_id_t));
    assert(recv_edges != NULL);

    /* send edges to each process */
    for (int i = 0; i < size; i++) {
        MPI_Irecv(&recv_edges[recv_offsets_edge[i]], recv_counts[i], MPI_DOUBLE, i, G->rank, MPI_COMM_WORLD, &request[i]);
    }   
    for (int i = 0; i < size; i++) {
        MPI_Send(&send_edges[send_offsets_edge[i]], send_counts[i], MPI_VERTEX_ID_T, i, i, MPI_COMM_WORLD);
    }
    MPI_Waitall(size, request, status);

    /* saving new value for local_m */
    local_m = counts;
    G->local_m = local_m;
    free(send_edges);
    free(recv_offsets_edge);
    free(send_offsets_edge);
    free(recv_counts);
    free(send_counts);

    for (edge_id_t i=0; i<2*G->local_m; i=i+2) {
        degree[recv_edges[i]]++;
    }
    /* Update graph data structure */
    G->endV = (vertex_id_t *) calloc(G->local_m, sizeof(vertex_id_t));
    assert(G->endV != NULL);
    G->rowsIndices = (edge_id_t *) malloc((local_n+1)*sizeof(edge_id_t));
    assert(G->rowsIndices != NULL);

    G->weights = (weight_t *) malloc(G->local_m * sizeof(weight_t));       
    assert(G->weights != NULL);
    G->rowsIndices[0] = 0; 
    for (vertex_id_t i=1; i<=G->local_n; i++) {
	    G->rowsIndices[i] = G->rowsIndices[i-1] + degree[i-1];
    }

    for (edge_id_t i=0; i<2*G->local_m; i=i+2) {
	    u = recv_edges[i+0];
	    v = recv_edges[i+1];
	    offset = degree[u]--;
	    G->endV[G->rowsIndices[u]+offset-1] = v;
	    G->weights[G->rowsIndices[u]+offset-1] = recv_weight[i/2];
    }

    free(recv_edges);
    free(degree);
    free(recv_weight);
}

