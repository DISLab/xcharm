#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include <stdarg.h>
#include <assert.h>
#include "defs.h"

using namespace std;

char outFilename[FNAME_LEN];
int nIters = 64;

void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s [--generate <type> -s <scale> | -in <filename>] [options] \n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    --generate <type> -- generate graph. Supported options of <type> are: SSCA2, RMAT\n");
    fprintf(stderr, "    -s <scale> -- number of vertices is 2^<scale>, in case of --generate option\n");
    fprintf(stderr, "    -in <filename> -- read graph from <filename>\n");
    fprintf(stderr, "    -out <output> -- algorithm result will be placed to <output> filename. It can be used in validation. <in>.mst is default value.\n");
    fprintf(stderr, "    -nIters <nIters> -- number of iterations. By default 64\n");
    fprintf(stderr, "    -h -- print usage\n");
}

void stop_program(int argc, char **argv, graph_t* G, const char* format, ...)
{
    if (G->rank == 0) { 
        va_list args;
        va_start(args, format);
        vfprintf(stderr,format, args);
        va_end(args);
        usage(argc, argv);
    }
    MPI_Finalize();
    exit(1);
}

void print0(int rank, const char* format, ...)
{
    if (rank == 0) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void init(int argc, char** argv, graph_t* G)
{
    MPI_Init (&argc, &argv);
    char graphFilename[FNAME_LEN] = "\0";
    bool should_read_graph=false;
    bool should_gen_ssca2=false;
    bool should_gen_rmat=false;
    G->scale = -1;
    G->directed = false;
    G->min_weight = 0.0;
    G->max_weight = 1.0;
    int l;
    bool no_file_name = true;
    MPI_Comm_size(MPI_COMM_WORLD, &G->nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &G->rank);
    G->scale=-1;

    if (argc == 1) stop_program(argc, argv, G,"");
    
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--generate")) {
            if (argc <= i+1) stop_program(argc, argv, G, "Select graph type\n");
            if (should_read_graph) {
                stop_program(argc, argv, G, ""); 
            }
            if (!strcmp(argv[i+1], "SSCA2")) {
                should_gen_ssca2 = true;
            } else {
                if (!strcmp(argv[i+1], "RMAT")) {
                    should_gen_rmat = true;
                } else {
                    stop_program(argc, argv, G,"Unknown graph type: %s\n", argv[i+1]);
                }
            }
            i++;
        }
        
        if (!strcmp(argv[i], "-in")) {
            if (should_gen_rmat || should_gen_ssca2) {
                stop_program(argc, argv, G, "");
            }
            should_read_graph=true;
            int l = strlen(argv[++i]);
            strncpy(graphFilename, argv[i], (l > FNAME_LEN-1 ? FNAME_LEN-1 : l) );
            if ( access(graphFilename, 0) ) stop_program(argc, argv, G, "no graph file: %s \n", graphFilename);
        }
        if (!strcmp(argv[i], "-s")) {
            if (argc <= i+1) stop_program(argc, argv, G, "");
            G->scale = (int)atoi(argv[++i]);
            G->n = (vertex_id_t)1 << G->scale;
            if (G->n % G->nproc != 0 ) {
                stop_program(argc, argv, G, "TotVertices(=2^%d) is not divided by nnodes(=%d) \n ", G->scale, G->nproc);
            }
        } 
        if (!strcmp(argv[i], "-out")) {
            if (argc <= i+1) stop_program(argc, argv, G, "");
            no_file_name = false;
            l = strlen(argv[++i]);
            strncpy(outFilename, argv[i], (l > FNAME_LEN-1 ? FNAME_LEN-1 : l) );
        }

        if (!strcmp(argv[i], "-h")) {
            stop_program(argc, argv, G,"");
        }
        if (!strcmp(argv[i], "-nIters")) {
            if (argc <= i+1) stop_program(argc, argv, G, "");
            nIters = (int) atoi(argv[++i]);
        }
    }
    if (should_gen_rmat) {
        if (G->scale == -1 ) {
            stop_program(argc, argv, G,"You must use -s <scale> with --gen <type>\n");
        }
        gen_RMAT_graph_MPI(G);
    } else { 
        if (should_gen_ssca2) {
            if (G->scale == -1 ) {
                stop_program(argc, argv, G,"You must use -s <scale> with --gen <type>\n");
            }
            gen_SSCA2_graph_MPI(G);
        } else { 
            if (should_read_graph) {
                readGraph_singleFile_MPI(G, graphFilename);
            } else {
                stop_program(argc, argv, G," ");
            }
        }
    }
    if (no_file_name) 
    {
	if(should_read_graph) sprintf(outFilename, "%s.mst", graphFilename);
	else 
	    if(should_gen_ssca2)sprintf(outFilename,"ssca2-%d.mst", G->scale);
	    else sprintf(outFilename,"rmat-%d.mst", G->scale); 
    }
}

void write_output_information_mpi(forest_t *trees, char *filename)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank == 0)
    {
        FILE *F = fopen(filename, "wb");
        assert(fwrite(&(trees->numTrees), sizeof(vertex_id_t), 1, F) == 1);
        assert(fwrite(&(trees->numEdges), sizeof(edge_id_t), 1, F) == 1);
        assert(fwrite(trees->p_edge_list, sizeof(edge_id_t), 2*trees->numTrees, F) == 2*trees->numTrees);
        assert(fwrite(trees->edge_id, sizeof(edge_id_t), trees->numEdges, F) == trees->numEdges);
        fclose(F);
    }
}



int main(int argc, char **argv) 
{
    graph_t g;
    double start_ts, finish_ts;
    double *perf;
    forest_t trees_output;

    init(argc, argv, &g);

    init_mst(&g);
    perf = (double *)malloc(nIters * sizeof(double));
    void* result = 0;

    print0(g.rank,"start algorithm iterations...\n");
    for (int i = 0; i < nIters; ++i) {
        print0(g.rank,"\tMST %d\t ...",i);
        MPI_Barrier(MPI_COMM_WORLD);
        start_ts = MPI_Wtime();
        result = MST(&g);
        finish_ts = MPI_Wtime();
        double time = finish_ts - start_ts;
        perf[i] = g.m / (1000000 * time);
        print0(g.rank,"\tfinished. Time is %.4f secs\n", time);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    print0(g.rank,"algorithm iterations finished.\n");
    
    convert_to_output(&g, result, &trees_output);    

    print0(g.rank,"converting finished\n");
    write_output_information_mpi(&trees_output, outFilename);
    if(!g.rank)free(trees_output.p_edge_list);
    if(!g.rank)free(trees_output.edge_id);

    print0(g.rank,"writing to file finished.\n");
    double min_perf, max_perf, avg_perf;
    double global_min_perf, global_max_perf, global_avg_perf;
    max_perf = avg_perf = 0;
    min_perf = DBL_MAX;     
    for (int i = 0; i < nIters; ++i) {  
        avg_perf += perf[i];
        if (perf[i] < min_perf) min_perf = perf[i]; 
        if (perf[i] > max_perf) max_perf = perf[i]; 
    }
    avg_perf /= nIters;
    
    MPI_Reduce(&min_perf, &global_min_perf, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max_perf, &global_max_perf, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&avg_perf, &global_avg_perf, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    print0(g.rank,"%s: vertices = %" PRIu32 " edges = %" PRIu64 " trees = %zu nIters = %d MST performance min = %.4f avg = %4f max = %.4f MTEPS\n",g.filename, g.n, g.m, trees_output.numTrees, nIters, min_perf, avg_perf, max_perf);
    print0(g.rank,"Performance = %.4f MTEPS\n", avg_perf);
    
    finalize_mst(&g);                                    
    freeGraph(&g);
    MPI_Finalize();
    return 0;
}
