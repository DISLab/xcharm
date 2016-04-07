#ifndef __GraphGenerator_h__
#define __GraphGenerator_h__

#include "GraphIO.h"

enum class GeneratorType {Random, RMAT, Kronecker};

template <class V>
class GraphGenerator : public GraphIO<V> {
	public:
		GraphGenerator() {}
};

template <class V>
class Random_GraphGenerator : public GraphGenerator<V> {
	private:
		uint64_t vertexId;
		uint64_t nVertex;
		uint64_t nEdges;
		uint16_t nScale, K;
	public:
		Random_GraphGenerator(uint16_t nScale, uint16_t K) : nScale(nScale), K(K) {
			vertexId = 0;
			nVertex = 1 << nScale;
		}
		uint64_t getVertexNum() { return nVertex; }
		uint64_t getEdgesNum() { return nEdges; }
		uint64_t getVertex(V& v) { 
			v.edge_dsts.clear();
			v.edge_weigths.clear();
			v.edge_ids.clear();

			for (uint16_t i = 0; i < K; i++) {			
				uint64_t vid;
				while ((vid = rand() % nVertex) == vertexId) {}
				v.edge_dsts.push_back(vid);
				v.edge_weigths.push_back(((double)rand())/RAND_MAX);
				v.edge_ids.push_back(0); //FIXME!
			}
			return vertexId++; 
		} 
};

#endif
