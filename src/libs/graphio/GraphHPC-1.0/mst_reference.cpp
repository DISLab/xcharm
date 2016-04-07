#include <iostream>
#include <vector>
#include <algorithm>
#include <float.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "defs.h"
using namespace std;

typedef struct
{
    vertex_id_t startV;
    vertex_id_t endV;
    weight_t w;
    vertex_id_t edge_id;
} edge_type;

extern "C" void init_mst(graph_t *G)
{
}   

/* MST reference implementation. Prim's algorithm 
 * NOTE: isolated vertex is also tree, such tree must be represented as separate element of trees vector with zero-length edges list */
typedef vector<vector<edge_id_t > > result_t;
result_t trees;
extern "C" void* MST(graph_t *G)
{
    trees.clear();
    edge_type nearest_edge;
    vector <uint8_t> marked_vert_in_forest(G->n, 0);
    bool non_visited_tree = true;
    vertex_id_t root = 0;
    vertex_id_t numTrees = 0;
    while (non_visited_tree) {
        vector <vertex_id_t> q_marked;
        q_marked.push_back(root);
        marked_vert_in_forest[root] = 1;


        bool non_visited_vert_in_tree = true;
        trees.push_back(vector <edge_id_t> ());
        while (non_visited_vert_in_tree) {
            non_visited_vert_in_tree = false;
            nearest_edge.w = DBL_MAX;
            for (vertex_id_t i = 0; i < q_marked.size(); i++) {
                for (edge_id_t j = G->rowsIndices[q_marked[i]]; j < G->rowsIndices[q_marked[i]+1]; j++) {
                    if (!marked_vert_in_forest[G->endV[j]]) {
                        non_visited_vert_in_tree = true;
                        if (nearest_edge.w == DBL_MAX || G->weights[j] < nearest_edge.w) {
                            nearest_edge.startV = q_marked[i];
                            nearest_edge.endV = G->endV[j];
                            nearest_edge.w = G->weights[j];
                            nearest_edge.edge_id = j;
                        }
                    }
                }
            }
            if (nearest_edge.w != DBL_MAX) {
                marked_vert_in_forest[nearest_edge.endV] = 1;
                trees[numTrees].push_back(nearest_edge.edge_id);
                q_marked.push_back(nearest_edge.endV);
            }
        }
        non_visited_tree = false;
        for (vertex_id_t i = 0; i < G->n ; i++) {
            if (!marked_vert_in_forest[i]) {
                non_visited_tree = true;
                root = i;
                numTrees++;
                break;
            }
        }
    }
    return &trees;
}

/* Convert MST implementation output to GraphHPC-2015 forest_t data type. forest_t will be used in validation
 * NOTE: isolated vertex is also tree, such tree must be represented as separate element of trees_mst vector with zero-length edges list 
 * FIXME: If you change MST output data structure, you must change this function */
extern "C" void convert_to_output(graph_t *G, void* result, forest_t *trees_output)
{
    result_t &trees_mst = *reinterpret_cast<result_t*>(result);
    trees_output->p_edge_list = (edge_id_t *)malloc(trees_mst.size()*2 * sizeof(edge_id_t));
    edge_id_t number_of_edges = 0;
    for (vertex_id_t i = 0; i < trees_mst.size(); i++) number_of_edges += trees_mst[i].size();
    trees_output->edge_id = (edge_id_t *)malloc(number_of_edges * sizeof(edge_id_t));
    trees_output->p_edge_list[0] = 0;
    trees_output->p_edge_list[1] = trees_mst[0].size();
    for (vertex_id_t i = 1; i < trees_mst.size(); i++) {
        trees_output->p_edge_list[2*i] = trees_output->p_edge_list[2*i-1];
        trees_output->p_edge_list[2*i +1] = trees_output->p_edge_list[2*i-1] + trees_mst[i].size();
    }
    int k = 0;
    for (vertex_id_t i = 0; i < trees_mst.size(); i++) {
        for (edge_id_t j = 0; j < trees_mst[i].size(); j++) {
            trees_output->edge_id[k] = trees_mst[i][j];
            k++;
        }
    }
     
    trees_output->numTrees = trees_mst.size();
    trees_output->numEdges = number_of_edges;
}

extern "C" void finalize_mst(graph_t *G)
{
}

