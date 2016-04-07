#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include "defs.h"
using namespace std;
char inFilename[FNAME_LEN];
char outFilename[FNAME_LEN];

int nIters = 64;
#if defined(CLOCK_MONOTONIC)
#define CLOCK CLOCK_MONOTONIC
#elif defined(CLOCK_REALTIME)
#define CLOCK CLOCK_REALTIME
#else
#error "Failed to find a timing clock."
#endif

void usage(int argc, char **argv)
{
    printf("Usage:\n");
    printf("    %s -in <input> [options] \n", argv[0]);
    printf("Options:\n");
    printf("    -in <input> -- input graph filename\n");
    printf("    -out <output> -- algorithm result will be placed to output filename. It can be used in validation. <in>.mst is default value.\n");
    printf("    -nIters <nIters> -- number of iterations. By default 64\n");
    exit(1);
}

void init(int argc, char** argv, graph_t* G)
{
    int l;
    inFilename[0] = '\0';
    outFilename[0] = '\0';
    bool no_in_filename = true; 

    if (argc == 1) usage(argc, argv);

    for (int i = 1; i < argc; ++i) {
   		if (!strcmp(argv[i], "-in")) {
            l = strlen(argv[++i]);
            strncpy(inFilename, argv[i], (l > FNAME_LEN-1 ? FNAME_LEN-1 : l) );
            no_in_filename = false;
        }
   		if (!strcmp(argv[i], "-out")) {
            l = strlen(argv[++i]);
            strncpy(outFilename, argv[i], (l > FNAME_LEN-1 ? FNAME_LEN-1 : l) );
        }
		if (!strcmp(argv[i], "-nIters")) {
			nIters = (int) atoi(argv[++i]);
        }
    }
    if (no_in_filename) usage(argc, argv);
    if (strlen(outFilename) == 0) sprintf(outFilename, "%s.mst", inFilename);
}


int main(int argc, char **argv) 
{
    graph_t g;
    struct timespec start_ts, finish_ts;
    double *perf;
    forest_t trees_output;
    /* initializing and reading the graph */
    init(argc, argv, &g); 
    readGraph(&g, inFilename);
    init_mst(&g);

    perf = (double *)malloc(nIters * sizeof(double));
    void *result = 0;

    printf("start algorithm iterations...\n");
    for (int i = 0; i < nIters; ++i) {
        printf("\tMST %d\t ...",i); fflush(NULL);
        clock_gettime(CLOCK, &start_ts);
        result = MST(&g);
        clock_gettime(CLOCK, &finish_ts);
        double time = (finish_ts.tv_nsec - (double)start_ts.tv_nsec) * 1.0e-9 + (finish_ts.tv_sec - (double)start_ts.tv_sec);
        perf[i] = g.m / (1000000 * time);
        printf("\tfinished. Time is %.4f secs\n", time);
    }
    printf("algorithm iterations finished.\n");

    convert_to_output(&g, result, &trees_output);
    write_output_information(&trees_output, outFilename);
    free(trees_output.p_edge_list);
    free(trees_output.edge_id);

    /* final print */
    double min_perf, max_perf, avg_perf;
    max_perf = avg_perf = 0;
    min_perf = DBL_MAX;
    for (int i = 0; i < nIters; ++i) {
    	avg_perf += perf[i];
        if (perf[i] < min_perf) min_perf = perf[i];
        if (perf[i] > max_perf) max_perf = perf[i];
    }
    avg_perf /= nIters;

    printf("%s: vertices = %d edges = %lld trees = %u nIters = %d MST performance min = %.4f avg = %.4f max = %.4f MTEPS\n", 
            inFilename, g.n, (long long)g.m, trees_output.numTrees, nIters, min_perf, avg_perf, max_perf);
    printf("Performance = %.4f MTEPS\n", avg_perf);
    free(perf);
    freeGraph(&g);
    finalize_mst(&g);

    return 0;
}
