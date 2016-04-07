#include <iostream>
#include <vector>
#include <algorithm>
#include <float.h>
#include <string.h>
#include <assert.h>
#include "defs.h"
char inFilename[FNAME_LEN];
char outFilename[FNAME_LEN];
double *valid_weight_trees;
vertex_id_t valid_num_trees;

using namespace std;

int compare (const void * a, const void * b)
{
    if ( *(weight_t*)a <  *(weight_t*)b ) return -1;
    if ( *(weight_t*)a == *(weight_t*)b ) return 0;
    if ( *(weight_t*)a >  *(weight_t*)b ) return 1;
    return 0;
}

void usage(int argc, char **argv)
{
    printf("%s generates data for validation\n", argv[0]);
    printf("Usage:\n");
    printf("    %s -in <input> [options]\n", argv[0]);
    printf("Options:\n");
    printf("    -in <input> -- input graph filename\n");
    printf("    -out <output> -- output filename (valid information)\n");
    exit(1);
}

void init(int argc, char** argv)
{
    int l;
    bool no_in_filename = true;
    inFilename[0] = outFilename[0] = '\0';
    
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
    }
    if (no_in_filename) usage(argc, argv);
    if (strlen(outFilename) == 0) sprintf(outFilename, "%s.vinfo", inFilename);
}

void write_valid_information()
{
    FILE *F = fopen(outFilename, "wb");
    assert(fwrite(&valid_num_trees, sizeof(vertex_id_t), 1, F) == 1);
    assert(fwrite(valid_weight_trees, sizeof(weight_t), valid_num_trees, F) == valid_num_trees);
    fclose(F);
}

int main(int argc, char **argv)
{
    graph_t g;
    init(argc, argv);
    readGraph(&g, inFilename);
    typedef vector < vector < edge_id_t > > result_t;
    result_t& trees = *reinterpret_cast<result_t*>(MST(&g));
    valid_weight_trees = (double *)malloc(sizeof(double) * trees.size());
    valid_num_trees = trees.size();

    for (vertex_id_t i = 0; i < valid_num_trees; i++) {
        weight_t tmp_weight = 0;
        for (edge_id_t j = 0; j < trees[i].size(); j++) {
            tmp_weight = tmp_weight + g.weights[trees[i][j]];
        }
        valid_weight_trees[i] = tmp_weight;
    }
    qsort (valid_weight_trees, trees.size(), sizeof(weight_t), compare);
    write_valid_information();
    printf("Valid information is written to %s\n", outFilename);
    return 0;
}


