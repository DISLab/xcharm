#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <error.h>
#include <unistd.h>
#include "defs.h"

#include <vector>

using namespace std;

#define NROOTS_DEFAULT 64
#define FNAME_LEN   256

char outFilename[FNAME_LEN];
vector<edge_id_t> edges;

void usage(int argc, char **argv)
{
    printf("SSCA2 graph generator\n");
    printf("Usage:\n");
    printf("    %s -s <scale> [options]\n", argv[0]);
    printf("Options:\n");
    printf("   -s <scale>, number of vertices is 2^<scale>\n");
    printf("   -out <filename>, ssca2-<s> is default value\n");
    exit(1);
}

void init (int argc, char** argv, graph_t* G)
{
    G->scale = -1;
    G->directed = false;
    G->min_weight = 0.0;
    G->max_weight = 1.0;
    int l;
    int no_file_name = 1;
    if (argc == 1) {
        usage(argc, argv);
    }
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-s")) {
            G->scale = (int)atoi(argv[++i]);
        }
        if (!strcmp(argv[i], "-out")) {
            no_file_name = 0;
            l = strlen(argv[++i]);
            strncpy(outFilename, argv[i], (l > FNAME_LEN-1 ? FNAME_LEN-1 : l) );
        }
        if (!strcmp(argv[i], "-h")) {
              usage(argc, argv);
        } 
    }    
    
    if (no_file_name) sprintf(outFilename, "ssca2-%d", G->scale);
    G->n = (vertex_id_t)1 << G->scale;
}

void gen_SSCA2_graph(graph_t* g)
{
    uint32_t TotVertices;
	uint32_t* clusterSizes;
	uint32_t* firstVsInCluster;
	uint32_t estTotClusters, totClusters;
	
	uint32_t *startVertex, *endVertex;
	uint32_t numEdges;
    uint32_t numIntraClusterEdges, numInterClusterEdges;
	weight_t* weights;
    weight_t MinWeight,MaxWeight; 
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


    /*----------------------------------------------*/
	/*		initialize RNG 		*/
	/*----------------------------------------------*/
    seed = 2387;   
    srand48(seed);
   
    MinWeight = g->min_weight;
    MaxWeight = g->max_weight;
    TotVertices = g->n; 

	/*----------------------------------------------*/
	/*		generate clusters		*/
	/*----------------------------------------------*/
    
    MaxCliqueSize = 49;

  	/* Estimate number of clusters required to make up 
	 * TotVertices and pad by 25% */
	estTotClusters = 1.25 * TotVertices / (MaxCliqueSize/2);
	
	/* Allocate a block of memory for this cluster-size array */
	clusterSizes = (uint32_t *) malloc(estTotClusters*sizeof(uint32_t));

	/* Generate random cluster sizes. */
	for(i = 0; i < estTotClusters; i++) {
		clusterSizes[i] = 1 + ( drand48() * MaxCliqueSize);
	}

	totClusters = 0;
  
	/* Allocate memory for storing the first vertex in each cluster */
	firstVsInCluster = (uint32_t *) malloc(estTotClusters*sizeof(uint32_t));

	/* Compute the first vertex in each cluster */
	firstVsInCluster[0] = 0;
	for (i=1; i<estTotClusters; i++) {
		firstVsInCluster[i] = firstVsInCluster[i-1] + clusterSizes[i-1];
		if (firstVsInCluster[i] > TotVertices-1)
			break;
	}

	totClusters = i;

	/* Fix the size of the last cluster */
	clusterSizes[totClusters-1] = TotVertices -\
		firstVsInCluster[totClusters-1];

	/*------------------------------------------------------*/
	/*		generate intra-cluster edges		*/
	/*------------------------------------------------------*/
 
	/* Roughly estimate the total number of edges */
	estNumEdges = (uint32_t) ((TotVertices * (double) MaxCliqueSize * (2-ProbUnidirectional)/2) +\
		      	        (TotVertices * (double) ProbIntercliqueEdges/(1-ProbIntercliqueEdges))) * \
				(1+MaxParallelEdges/2);

//	fprintf(stderr, "\n\tscale = %d MaxCliqueSize = %d Estimated number of edges = %d\n", g->scale, MaxCliqueSize, estNumEdges);
//	fprintf(stderr, "Generating intra-cluster edges ... ");

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
	for (i_cluster=0; i_cluster < totClusters; i_cluster++) {

		for (i = 0; i < clusterSizes[i_cluster]; i++) {

			for (j = 0; j < i; j++) {
			
				/* generate an edge only in 
				 * one direction */
				for (k=0; k<1+((uint32_t) (MaxParallelEdges-1) *  drand48()); k++) {
					startV[edgeNum] = j + \
					firstVsInCluster[i_cluster];	
					endV[edgeNum] = i + \
					firstVsInCluster[i_cluster];
					edgeNum++;
				}	
			}
			
		}
	}

	numIntraClusterEdges = edgeNum;

//	fprintf(stderr, "done\n");
//	printf("\tNo. of intra-cluster edges - %d \n", numIntraClusterEdges);
	
	/*----------------------------------------------*/
	/*		connect the clusters		*/
	/*----------------------------------------------*/

//        fprintf(stderr, "Generating inter-clique edges ... ");	

	/* generate exponential distances as suggested in the spec */
	dsize = (uint32_t) (log((double)TotVertices)/log(2));
	d = (uint32_t *) malloc(dsize * sizeof(uint32_t));
	for (i = 0; i < dsize; i++) {
		d[i] = (uint32_t) pow(2, (double) i);
	}

	currCluster = 0;

	for (i = 0; i < TotVertices; i++) {

		p = ProbIntercliqueEdges;	

		/* determine current cluster */
		for (j = currCluster; j<totClusters; j++) {
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
						startV[edgeNum] = i;
						endV[edgeNum] = j;
						edgeNum++;	
					}	
				}	
			}
			
			p = p/2;
		}
	}
	
	numEdges = edgeNum;
	numInterClusterEdges = numEdges - numIntraClusterEdges;	

	free(clusterSizes);  
	free(firstVsInCluster);
	free(d);

//	fprintf(stderr, "done\n");
//	fprintf(stderr, "\tNo. of inter-cluster edges - %d\n", numInterClusterEdges);
//	fprintf(stderr, "\tTotal no. of edges - %d\n", numEdges);

	/*--------------------------------------------------------------*/
	/*		shuffle vertices to remove locality		*/
	/*--------------------------------------------------------------*/

//	fprintf(stderr, "Shuffling vertices to remove locality ... ");
#if DEBUG
	fprintf (stderr, "[allocating %3.3f GB memory ... ", (double) (TotVertices+2*numEdges)*8/(1<<30));
#endif
	permV = (uint32_t *) malloc(TotVertices*sizeof(uint32_t));
	startVertex = (uint32_t *) malloc(numEdges*sizeof(uint32_t));
	endVertex = (uint32_t *) malloc(numEdges*sizeof(uint32_t));
#if DEBUG
	fprintf(stderr, "done] ");
#endif
	for(i=0; i<TotVertices; i++) {
		permV[i] = i;
	}

	/* Permute the vertices and store them in permV */
	for (i=0; i<TotVertices; i++) {

		t1 = i +  drand48() * (TotVertices - i);

		if (t1 != i) {
			t2 = permV[t1];
			permV[t1]=permV[i];
			permV[i]=t2;
		}
	
	}

	for (i=0; i<numEdges; i++) {
		startVertex[i] = permV[startV[i]];
		endVertex[i] = permV[endV[i]];
	}

	free(startV);
	free(endV);	
	free(permV);

//	fprintf(stderr, "done\n");

	/*----------------------------------------------*/
    /*              generate edge weights           */
    /*----------------------------------------------*/

//    fprintf(stderr, "Generating edge weights ... ");

    weights = (weight_t *) malloc(numEdges*sizeof(weight_t));

    for (i=0; i<numEdges; i++) {
        weights[i] = MinWeight + (weight_t) (MaxWeight - MinWeight) * drand48();
    }

//    fprintf(stderr, "done\n");
	
	
//	g->start = startVertex;
//	g->end = endVertex;
	g->weights = weights;
	g->n = TotVertices;

//    fprintf(stderr, "Make dests ... ");    
    vector<vector<uint32_t> > dests(TotVertices);
    vector<vector<weight_t> > weight_vect(TotVertices);
    for (uint32_t i=0; i<numEdges; i++) {
        /* direct edge */
        dests[startVertex[i]].push_back(endVertex[i]);
        weight_vect[startVertex[i]].push_back(weights[i]);

        /* back edge */
        dests[endVertex[i]].push_back(startVertex[i]);
        weight_vect[endVertex[i]].push_back(weights[i]);
    }
//    fprintf(stderr, "done\n");
    free(startVertex);
    free(endVertex);
    free(weights);
	g->m = 2*numEdges;
    FILE *F = fopen(outFilename, "wb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", outFilename);
    assert(fwrite(&g->n, sizeof(vertex_id_t), 1, F) == 1);
    assert(fwrite(&g->m, sizeof(edge_id_t), 1, F) == 1);
    assert(fwrite(&g->directed, sizeof(bool), 1, F) == 1);
    uint8_t align = 0;
    assert(fwrite(&align, sizeof(uint8_t), 1, F) == 1);
    uint64_t l = 0;
    assert(fwrite(&l, sizeof(edge_id_t), 1, F) == 1);
    for(uint32_t i = 0; i < TotVertices; ++i) { 
        l+=dests[i].size();
        assert(fwrite(&l, sizeof(edge_id_t), 1, F) == 1);
    }
    for(uint32_t i = 0; i < TotVertices; ++i) {
        assert(fwrite(&dests[i][0], sizeof(vertex_id_t), dests[i].size(), F) == dests[i].size());
    }
    for(uint32_t i = 0; i < TotVertices; ++i) {
        assert(fwrite(&weight_vect[i][0], sizeof(weight_t), weight_vect[i].size(), F) == weight_vect[i].size());
    }
    fclose(F);
}

int main (int argc, char** argv)
{
    graph_t g;
    
    init(argc, argv, &g);
    
    gen_SSCA2_graph(&g);
    
    return 0;
}
