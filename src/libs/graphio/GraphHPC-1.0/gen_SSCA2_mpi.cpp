#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <error.h>
#include <unistd.h>
#include <mpi.h>
#include <unistd.h>
#include "defs.h"

void gen_SSCA2_graph_MPI(graph_t* g)
{
    uint32_t local_TotVertices, TotVertices;
	uint32_t* clusterSizes;
	uint32_t* firstVsInCluster;
	uint32_t estTotClusters, local_totClusters;
	
	uint32_t *startVertex, *endVertex;
	uint32_t local_numEdges;
	weight_t* weights;
    weight_t MinWeight,MaxWeight;
    vertex_id_t u, v; 
    edge_id_t offset;
    uint32_t MaxCliqueSize;
    uint32_t MaxParallelEdges = 1;
    double ProbUnidirectional = 1.0;
    double ProbIntercliqueEdges = 0.1;
	uint32_t i_cluster, currCluster;
	uint32_t *startV, *endV, *d;
	uint32_t estNumEdges, edgeNum;

	uint32_t i, j, k, t, t1, t2, dsize;
	double p;
    int seed;
	uint32_t* permV;
    int size, rank, lgsize;
    

    g->directed = false;
    g->min_weight = 0;
    g->max_weight = 1;
    g->filename[0] = '\0';
    g->n = (vertex_id_t)1 << g->scale;
    if (strlen(g->filename) == 0) sprintf(g->filename, "ssca2-%d", g->scale);

    /*----------------------------------------------*/
	/*		initialize RNG 		*/
	/*----------------------------------------------*/
    seed = 2387;   
    srand48(seed);
   
    MinWeight = g->min_weight;
    MaxWeight = g->max_weight;
    TotVertices = g->n;
    size = g->nproc;
    g->local_n = g->n/size;
    local_TotVertices = g->local_n;
    rank = g->rank;
    for (lgsize = 0; lgsize < size; ++lgsize) {
        if ((1 << lgsize) == size) break;
    }
    
    MPI_Datatype MPI_VERTEX_ID_T;                                        
    MPI_Type_contiguous(sizeof(vertex_id_t), MPI_BYTE, &MPI_VERTEX_ID_T);
    MPI_Type_commit(&MPI_VERTEX_ID_T);                                   


	/*----------------------------------------------*/
	/*		generate local clusters		*/
	/*----------------------------------------------*/
    MaxCliqueSize = 49;

  	/* Estimate number of clusters required to make up 
	 * TotVertices and pad by 25% */
	estTotClusters = 1.25 * TotVertices / (size * MaxCliqueSize/2);
	
	/* Allocate a block of memory for this cluster-size array */
	clusterSizes = (uint32_t *) malloc(estTotClusters*sizeof(uint32_t));

	/* Generate random cluster sizes. */
    
	for(i = 0; i < estTotClusters; i++) {
		clusterSizes[i] = 1 + ( drand48() * MaxCliqueSize);
	}

	local_totClusters = 0;
  
	/* Allocate memory for storing the first vertex in each cluster */
	firstVsInCluster = (uint32_t *) malloc(estTotClusters*sizeof(uint32_t));

	/* Compute the first vertex in each cluster */
	firstVsInCluster[0] = 0;
	for (i=1; i<estTotClusters; i++) {
		firstVsInCluster[i] = firstVsInCluster[i-1] + clusterSizes[i-1];
		if (firstVsInCluster[i] > local_TotVertices-1)
			break;
	}

	local_totClusters = i;

	/* Fix the size of the last cluster */
	clusterSizes[local_totClusters-1] = local_TotVertices -\
		firstVsInCluster[local_totClusters-1];

	/*------------------------------------------------------*/
	/*		generate intra-cluster edges		*/
	/*------------------------------------------------------*/
 
	/* Roughly estimate the total number of edges */
	estNumEdges = (uint32_t) ((local_TotVertices * (double) MaxCliqueSize * (2-ProbUnidirectional)/2) +\
		      	        (local_TotVertices * (double) ProbIntercliqueEdges/(1-ProbIntercliqueEdges))) * \
				(1+MaxParallelEdges/2);

	/* Check if no. of edges is within bounds for 32-bit code */
	if ((estNumEdges > ((1<<30) - 1)) && (sizeof(uint32_t*) < 8)) {
		fprintf(stderr, "ERROR: long* should be 8 bytes \
				for this problem size\n");
		fprintf(stderr, "\tPlease recompile the code \
			       	in 64-bit mode\n");
		exit(-1);
	}

	edgeNum = 0;
	p = ProbUnidirectional;
#if DEBUG
	fprintf (stderr, "[allocating %3.3f GB memory ... ", (double) 2*estNumEdges*8/(1<<30));
#endif
	startV = (uint32_t *) malloc(estNumEdges*sizeof(uint32_t));
	endV = (uint32_t *) malloc(estNumEdges*sizeof(uint32_t));
#if DEBUG
	fprintf(stderr, "done] ");  
#endif
	for (i_cluster=0; i_cluster < local_totClusters; i_cluster++) {

		for (i = 0; i < clusterSizes[i_cluster]; i++) {

			for (j = 0; j < i; j++) {
			
				/* generate an edge only in 
				 * one direction */
					startV[edgeNum] = VERTEX_TO_GLOBAL( j + \
					firstVsInCluster[i_cluster], TotVertices, size, rank);	
					endV[edgeNum] = VERTEX_TO_GLOBAL( i + \
					firstVsInCluster[i_cluster], TotVertices, size, rank);
					edgeNum++;
			}
			
		}
	}


	/*----------------------------------------------*/
	/*		connect the clusters		*/
	/*----------------------------------------------*/

	/* generate exponential distances as suggested in the spec */
	dsize = (uint32_t) (log((double)TotVertices)/log(2));
	d = (uint32_t *) malloc(dsize * sizeof(uint32_t));
	for (i = 0; i < dsize; i++) {
		d[i] = (uint32_t) pow(2, (double) i);
	}

	currCluster = 0;

	for (i = 0; i < local_TotVertices; i++) {

		p = ProbIntercliqueEdges;	

		/* determine current cluster */
		for (j = currCluster; j<local_totClusters; j++) {
			if ((i >= firstVsInCluster[j]) && \
			    (i < firstVsInCluster[j] + clusterSizes[j])) {
				currCluster = j;
				break;
			}	
		}

		for (t = 1; t < dsize; t++) {

			j = (i + d[t] + (uint32_t) ( drand48() * (d[t] - d[t-1]))) % TotVertices;	

			/* Ensure that i and j don't belong to the same cluster */
	
			if ((j<firstVsInCluster[currCluster]) || \
			    (j>=firstVsInCluster[currCluster] + clusterSizes[currCluster])) {
				
				for (k=0; k<1+((uint32_t) (MaxParallelEdges - 1)*  drand48()); k++) {
					if (p >  drand48()) {
						startV[edgeNum] = VERTEX_TO_GLOBAL(i, TotVertices, size, rank);
						endV[edgeNum] = j;
						edgeNum++;	
					}	
				}	
			}
			
			p = p/2;
		}
	}
	
	local_numEdges = edgeNum;

	free(clusterSizes);  
	free(firstVsInCluster);
	free(d);

	/*--------------------------------------------------------------*/
	/*		shuffle vertices to remove locality		*/
	/*--------------------------------------------------------------*/

#if DEBUG
	fprintf (stderr, "[allocating %3.3f GB memory ... ", (double) (TotVertices+2*numEdges)*8/(1<<30));
#endif
	permV = (uint32_t *) malloc(local_TotVertices*sizeof(uint32_t));
	startVertex = (uint32_t *) malloc(local_numEdges*sizeof(uint32_t));
	endVertex = (uint32_t *) malloc(local_numEdges*sizeof(uint32_t));
#if DEBUG
	fprintf(stderr, "%d: done ", rank);
#endif
	for(i=0; i<local_TotVertices; i++) {
		permV[i] = i;
	}

	/* Permute the vertices and store them in permV */
	for (i=0; i<local_TotVertices; i++) {

		t1 = i +  drand48() * (local_TotVertices - i);

		if (t1 != i) {
			t2 = permV[t1];
			permV[t1]=permV[i];
			permV[i]=t2;
		}
	
	}

	for (i=0; i<local_numEdges; i++) {  
		startVertex[i] = VERTEX_TO_GLOBAL(permV[VERTEX_LOCAL(startV[i], TotVertices, size, rank)], TotVertices, size, rank);
        if (VERTEX_OWNER(endV[i], TotVertices, size) != rank) {
		    endVertex[i] = endV[i];
        } else {
       		endVertex[i] = permV[VERTEX_LOCAL(endV[i], TotVertices, size, rank)];
        }
	}

	free(startV);
	free(endV);	
	free(permV);

	/*----------------------------------------------*/
    /*              generate edge weights           */
    /*----------------------------------------------*/

    weights = (weight_t *) malloc(local_numEdges*sizeof(weight_t));

    for (i=0; i<local_numEdges; i++) {
        weights[i] = MinWeight + (weight_t) (MaxWeight - MinWeight) * drand48();
    }

    g->weights = weights;
	g->n = TotVertices;
	
    /*----------------------------------------------*/
    /*          calc data to send          */
    /*----------------------------------------------*/
    weight_t *send_weight = (weight_t*) malloc (2*local_numEdges * sizeof(weight_t));
    assert(send_weight != NULL);
    weight_t *recv_weight;
    vertex_id_t* send_edges = (vertex_id_t*) malloc (4 * local_numEdges * sizeof(vertex_id_t));
    assert(send_edges != NULL);
    vertex_id_t* recv_edges;
    int* recv_counts = (int*) calloc (size, sizeof(int));
    assert(recv_counts != NULL);
    int* send_counts = (int*) calloc (size, sizeof(int));
    assert(send_counts != NULL);    
    edge_id_t* send_offsets_edge = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(send_offsets_edge != NULL);
    edge_id_t* send_offsets_weight = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(send_offsets_weight != NULL);
    edge_id_t* recv_offsets_weight = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(recv_offsets_weight != NULL);
    edge_id_t* recv_offsets_edge = (edge_id_t*) calloc (size, sizeof(edge_id_t));
    assert(recv_offsets_edge != NULL);

    for (i=0; i<local_numEdges; i++) {
        int proc_id = VERTEX_OWNER(endVertex[i], TotVertices, size);
        send_counts[proc_id]++;
    }
    
    /*  calc offsets  */
    for (int i=1; i<size; i++) {
        send_offsets_edge[i] = send_offsets_edge[i-1] + 2 * send_counts[i-1];
        send_offsets_weight[i] = send_offsets_weight[i-1] + send_counts[i-1];
    }   
    for (int i=0; i<size; i++) {
        send_counts[i] = 0;
    }
    /* copy edges and weight to send_data */
    for (edge_id_t i=0; i<local_numEdges; i++) {
        int proc_id = VERTEX_OWNER(startVertex[i], TotVertices, size);
        offset = send_offsets_edge[proc_id] + 2 * send_counts[proc_id];
        send_edges[offset + 0] = startVertex[i];
        send_edges[offset + 1] = endVertex[i];
        offset = send_offsets_weight[proc_id] + send_counts[proc_id];
        send_weight[offset] = weights[i];
        send_counts[proc_id]++;
         
        proc_id = VERTEX_OWNER(endVertex[i], TotVertices, size);
        offset = send_offsets_edge[proc_id] + 2*send_counts[proc_id];
        send_edges[offset + 0] = endVertex[i];
        send_edges[offset + 1] = startVertex[i];
        offset = send_offsets_weight[proc_id] + send_counts[proc_id];
        send_weight[offset] = weights[i];
        send_counts[proc_id]++; 
    }
    free(startVertex);
    free(endVertex);
    free(weights);
    
    MPI_Request request[size];
    MPI_Status status[size];

    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
    
    for (int i=1; i<size; i++) {
        recv_offsets_weight[i] = recv_offsets_weight[i-1] + recv_counts[i-1];
    }
    edge_id_t counts = 0;
    for(int i=0; i<size; i++) {
        counts += recv_counts[i];
    }
    recv_weight = (weight_t*) malloc (counts * sizeof(weight_t));
    assert(recv_weight != NULL);
    
    MPI_Allreduce(&counts, &g->m, 1, MPI_UINT64_T, MPI_SUM, MPI_COMM_WORLD); 

    /* send weights to each process */
    for (int i = 0; i < size; i++) {
        MPI_Irecv(&recv_weight[recv_offsets_weight[i]], recv_counts[i], MPI_DOUBLE, i, rank, MPI_COMM_WORLD, &request[i]);
    }
    for (int i = 0; i < size; i++) {
        MPI_Send(&send_weight[send_offsets_weight[i]], send_counts[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD);
    }
    MPI_Waitall(size, request, status);

    free(send_weight);
    free(send_offsets_weight);
    free(recv_offsets_weight);

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
        MPI_Irecv(&recv_edges[recv_offsets_edge[i]], recv_counts[i], MPI_DOUBLE, i, rank, MPI_COMM_WORLD, &request[i]);
    }
    for (int i = 0; i < size; i++) {
        MPI_Send(&send_edges[send_offsets_edge[i]], send_counts[i], MPI_VERTEX_ID_T, i, i, MPI_COMM_WORLD);
    }
    MPI_Waitall(size, request, status);

    g->local_m = counts;
    vertex_id_t * degree = (vertex_id_t *) calloc(g->local_n, sizeof(vertex_id_t));
    assert(degree != NULL);
    for (edge_id_t i=0; i<2*g->local_m; i=i+2) {
        degree[VERTEX_LOCAL(recv_edges[i], TotVertices, size, rank)]++;
    }
    
    g->endV = (vertex_id_t *) calloc(g->local_m, sizeof(vertex_id_t));   
    assert(g->endV != NULL);                                             
    g->rowsIndices = (edge_id_t *) malloc((g->local_n+1)*sizeof(edge_id_t));
    assert(g->rowsIndices != NULL);                                      
                                                                     
    g->weights = (weight_t *) malloc(g->local_m * sizeof(weight_t));     
    assert(g->weights != NULL);                                          
    g->rowsIndices[0] = 0;                                               
    for (vertex_id_t i=1; i<=g->local_n; i++) {                          
        g->rowsIndices[i] = g->rowsIndices[i-1] + degree[i-1];           
    }                                                                    
                                                                     
    for (edge_id_t i=0; i<2*g->local_m; i=i+2) {                         
        u = VERTEX_LOCAL(recv_edges[i+0], TotVertices, size, rank); 
        v = recv_edges[i+1];                                             
        offset = degree[u]--;                                            
        g->endV[g->rowsIndices[u]+offset-1] = v;                         
        g->weights[g->rowsIndices[u]+offset-1] = recv_weight[i/2];
    }    
    free(recv_edges);                                                    
    free(degree);                                                        
    free(recv_weight);                                                   

}

