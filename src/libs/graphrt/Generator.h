#ifndef __Generator_h__
#define __Generator_h__

#include "DataTypes.h"

#define YIELD_THRASHOLD	1024

namespace GraphLib {

	inline unsigned int __log2p2(unsigned int n) {
		int l = 0;
		while (n >>= 1) l++;
		return l;
	}

	template <class Graph, class Opts, VertexMapping vertexMapping>
		class Base_Generator_Charm;

	template <class Graph, class Opts>
		class Base_Generator_Charm<Graph, Opts, VertexMapping::SingleVertex> {
			public:
				Base_Generator_Charm(Graph & g) : g(g), graphProxy(g.getProxy()) {}
				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					if (g.directed)
						graphProxy[e.first].connectVertex(Edge(e.second, w));
					else {
						graphProxy[e.first].connectVertex(Edge(e.second, w));
						graphProxy[e.second].connectVertex(Edge(e.first, w));
					}
				}
			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Graph graphProxy;
		};


	template <class Graph, class Opts>
		class Base_Generator_Charm<Graph, Opts, VertexMapping::MultiVertex> {
			public:
				Base_Generator_Charm(Graph & g) : g(g), graphProxy(g.getProxy()) {}
				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					if (g.directed)
						graphProxy[Graph::base(e.first)].connectVertex(std::make_pair(e.first, Edge(e.second, w)));
					else {
						graphProxy[Graph::base(e.first)].connectVertex(std::make_pair(e.first, Edge(e.second, w)));
						graphProxy[Graph::base(e.second)].connectVertex(std::make_pair(e.second, Edge(e.first, w)));
					}
				}
			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Graph graphProxy;
		};

	template <class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator>
		class Base_Generator_Tram;

	template <class Graph, class Opts, class CProxy_Aggregator>
		class Base_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, CProxy_Aggregator> {
			public:
				Base_Generator_Tram(Graph & g, CProxy_Aggregator & aggregator) : g(g), aggregator(aggregator) {}
				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter>
						* localAggregator = aggregator.ckLocalBranch();
					if (g.directed) {
						localAggregator->insertData(Edge(e.second, w), e.first);
					} else {
						localAggregator->insertData(Edge(e.second, w), e.first);
						localAggregator->insertData(Edge(e.first, w), e.second);
					}
				}
			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Aggregator aggregator;
		};

	template <class Graph, class Opts, class CProxy_Aggregator>
		class Base_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, CProxy_Aggregator> {
			public:
				Base_Generator_Tram(Graph & g, CProxy_Aggregator & aggregator) : g(g), aggregator(aggregator) {}
				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					ArrayMeshStreamer<std::pair<CmiUInt8, Edge>, long long, Vertex, SimpleMeshRouter>
						* localAggregator = aggregator.ckLocalBranch();
					if (g.directed) {
						localAggregator->insertData(std::make_pair(e.first, Edge(e.second, w)), Graph::base(e.first));
					} else {
						localAggregator->insertData(std::make_pair(e.first, Edge(e.second, w)), Graph::base(e.first));
						localAggregator->insertData(std::make_pair(e.second, Edge(e.first, w)), Graph::base(e.second));
					}
				}
			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Aggregator aggregator;
		};

	/**
	 * Graph template class
	 **/
	//template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
	//	class Generator;

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
		class Generator {
			protected:
				Graph g;
				Opts opts;
			public:
				Generator (Graph & g, Opts & opts) : g(g), opts(opts) {}
				virtual void addEdge(const std::pair<uint64_t,uint64_t> & e) = 0;
				void do_generate() {}
		};

}

#endif
