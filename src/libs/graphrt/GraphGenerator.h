#include <iostream>
#include <graph500/graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>
#include "NDMeshStreamer.h"
#include "DataTypes.h"
#include "Generator.h"
#include "GraphGenerator.decl.h"

namespace GraphLib {

	// Graph generator template
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType, 
					 GraphGeneratorType graphGeneratorType, TransportType transportType>
	class GraphGenerator;

	// GraphGenerator 
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType, 
					 GraphGeneratorType graphGeneratorType>
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, graphGeneratorType, TransportType::Charm> {
		typedef CProxy_Generator_Charm<Graph, Opts, vertexMapping,  graphType, graphGeneratorType> CProxy_Generator; 
		CProxy_Generator generator;
		public:
			GraphGenerator (Graph & g, Opts & opts) {
				generator = CProxy_Generator::ckNew(g, opts);
			}
			void generate() { generator.generate(); }
	};

	template <typename Graph, typename Opts, GraphType graphType, 
					 GraphGeneratorType graphGeneratorType>
	class GraphGenerator<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate() { 
				CkCallback startCb(CkIndex_Generator::generate(), generator), fooCb;
				aggregator.init(graphProxy.ckGetArrayID(), startCb, fooCb, -1, true);
			}
	};

	template <typename Graph, typename Opts, GraphType graphType, 
					 GraphGeneratorType graphGeneratorType>
	class GraphGenerator<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<std::pair<CmiUInt8, Edge>, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate() { 
				CkCallback startCb(CkIndex_Generator::generate(), generator), fooCb;
				aggregator.init(graphProxy.ckGetArrayID(), startCb, fooCb, -1, true);
			}
	};

	//
	template <class Graph, class Opts, VertexMapping vertexMapping, GraphType graphType, GraphGeneratorType graphGeneratorType>
		class Generator_Charm;

	template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType>
		class Generator_Charm<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType > : 
			public CBase_Generator_Charm <Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType>, 
			Generator<Graph, Opts, graphGeneratorType> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				public:
					Generator_Charm(Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()), 
						Generator<Graph, Opts, graphGeneratorType>(g, opts) {}
					void generate() { Generator<Graph, Opts, graphGeneratorType>::do_generate(); }
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();
						graphProxy[e.first].connectVertex(Edge(e.second, w));
						graphProxy[e.second].connectVertex(Edge(e.first, w));
					}
			};

	template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType>
		class Generator_Charm<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType > : 
			public CBase_Generator_Charm <Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType>, 
			Generator<Graph, Opts, graphGeneratorType> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				public:
					Generator_Charm(Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()), 
						Generator<Graph, Opts, graphGeneratorType>(g, opts) {}
					void generate() { Generator<Graph, Opts, graphGeneratorType>::do_generate(); }
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						CkAbort("function is not implemented");
						//double w = drand48();
						//graphProxy[e.first].connectVertex(Edge(e.second, w));
						//graphProxy[e.second].connectVertex(Edge(e.first, w));
					}
			};

	//
	//template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType>
	//	void Generator_Charm<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType>::addEdge(std::pair<uint64_t,uint64_t> & e) {} 
	//template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType>
	//	void Generator_Charm<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType>::addEdge(std::pair<uint64_t,uint64_t> & e) {} 

	template <class Graph, class Opts, VertexMapping vertexMapping, GraphType graphType, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram;

	template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType, CProxy_Aggregator> : 
			public CBase_Generator_Tram <Graph, Opts, VertexMapping::SingleVertex, graphType, graphGeneratorType, CProxy_Aggregator >, 
			Generator<Graph, Opts, graphGeneratorType> {
				typedef typename Graph::Proxy GraphProxy;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;

				Graph g;
				CProxy_Aggregator aggregator;
				public:
					Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : 
						g(g), aggregator(aggregator), Generator<Graph, Opts, graphGeneratorType>(g, opts) {}
					void generate() { Generator<Graph, Opts, graphGeneratorType>::do_generate(); }
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();

						//CkPrintf("adding edge (%lld, %lld)\n", e.first, e.second);
						ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter>
							* localAggregator = aggregator.ckLocalBranch();
						localAggregator->insertData(Edge(e.second, w), e.first);
						//localAggregator->insertData(Edge(e.first, w), e.second);
					}
			};

	template <class Graph, class Opts, GraphType graphType, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType, CProxy_Aggregator> : 
			public CBase_Generator_Tram <Graph, Opts, VertexMapping::MultiVertex, graphType, graphGeneratorType, CProxy_Aggregator >, 
			Generator<Graph, Opts, graphGeneratorType> {
				typedef typename Graph::Proxy GraphProxy;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;

				Graph g;
				Opts opts;
				CProxy_Aggregator aggregator;
				public:
					Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : 
						g(g), opts(opts), aggregator(aggregator), Generator<Graph, Opts, graphGeneratorType>(g, opts) {}
					void generate() { Generator<Graph, Opts, graphGeneratorType>::do_generate(); }
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();
						ArrayMeshStreamer<std::pair<CmiUInt8, Edge>, long long, Vertex, SimpleMeshRouter>
							* localAggregator = aggregator.ckLocalBranch();
						localAggregator->insertData(std::make_pair(e.first, Edge(e.second, w)), e.first / (opts.N / CmiNumPes()) 
								/*FIXME: need common mapping function here*/);
						//localAggregator->insertData(std::make_pair(e.second, Edge(e.first, w)), e.second / (opts.N / CmiNumPes())
						//		/*FIXME: need common mapping function here*/);
					}
			};

}

#define CK_TEMPLATES_ONLY
#include "GraphGenerator.def.h"
#undef CK_TEMPLATES_ONLY

