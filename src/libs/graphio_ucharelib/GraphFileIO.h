#ifndef __GraphFileIO_h__
#define __GraphFileIO_h__
#include <string>
#include <charm++.h>
#include "GraphIO.h"
//#include "GraphHPC-1.0/defs.h"
#include "GraphHPC-0.2/defs.h"

/*** FIXME: this declaration from GraphHPC-1.0/def.h **/
typedef struct
{
    vertex_id_t numTrees;
    edge_id_t numEdges;
    edge_id_t* p_edge_list;
    edge_id_t* edge_id;

} forest_t;
void write_output_information(forest_t *trees, char *filename);

/* Abstrac class just for interface */
template <class V>
class GraphFileIO : public GraphIO<V> {
	public:
		GraphFileIO(std::string &filename) {}
		~GraphFileIO() {}
};

// Supported dataset formats
enum class GraphFileIOFormat {GraphHPC};

/* GraphHPC format file reader/writer */
template <class V>
class GraphHPC_GraphFileIO : public GraphFileIO<V> {
	private:
		std::string filename;
		graph_t G;
		uint64_t vertexId;

	public:
		GraphHPC_GraphFileIO(std::string filename) : filename(filename), vertexId(0), GraphFileIO<V>(filename) {
			CkPrintf("Reading %s\n", filename.c_str());
			readGraph(&G, (char *)filename.c_str());
		}
		uint64_t getVertexNum() { return G.n; }
		uint64_t getEdgesNum() { return G.m; }
		uint64_t getVertex(V& v) { 
			v.edge_dsts.clear();
			v.edge_weigths.clear();
			v.edge_ids.clear();

		  for (edge_id_t j = G.rowsIndices[vertexId]; j < G.rowsIndices[vertexId+1]; j++) {
				v.edge_dsts.push_back(G.endV[j]);
				v.edge_weigths.push_back(G.weights[j]);
				v.edge_ids.push_back(j);
			}	
			return vertexId++; 
		} 

		static void storeTreeToFile(std::string filename, forest_t *trees_output) {
			write_output_information(trees_output, (char *)filename.c_str());
		}
};

#endif
