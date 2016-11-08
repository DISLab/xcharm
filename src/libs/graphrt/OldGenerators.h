#ifndef __OLDGENERATORS_H__
#define __OLDGENERATORS_H__

namespace GraphLib {
	template <class Graph, class Opts, VertexMapping vertexMapping, GraphGeneratorType graphGeneratorType>
		class Generator_Charm;

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
		class Generator_Charm<Graph, Opts, VertexMapping::SingleVertex, graphGeneratorType > : 
			public CBase_Generator_Charm <Graph, Opts, VertexMapping::SingleVertex, graphGeneratorType>, 
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

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
		class Generator_Charm<Graph, Opts, VertexMapping::MultiVertex, graphGeneratorType > : 
			public CBase_Generator_Charm <Graph, Opts, VertexMapping::MultiVertex, graphGeneratorType>, 
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
	//template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
	//	void Generator_Charm<Graph, Opts, VertexMapping::SingleVertex, graphGeneratorType>::addEdge(std::pair<uint64_t,uint64_t> & e) {} 
	//template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
	//	void Generator_Charm<Graph, Opts, VertexMapping::MultiVertex, graphGeneratorType>::addEdge(std::pair<uint64_t,uint64_t> & e) {} 

	template <class Graph, class Opts, VertexMapping vertexMapping, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram;

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphGeneratorType, CProxy_Aggregator> : 
			public CBase_Generator_Tram <Graph, Opts, VertexMapping::SingleVertex, graphGeneratorType, CProxy_Aggregator >, 
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

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType, class CProxy_Aggregator >
		class Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphGeneratorType, CProxy_Aggregator> : 
			public CBase_Generator_Tram <Graph, Opts, VertexMapping::MultiVertex, graphGeneratorType, CProxy_Aggregator >, 
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

};


#endif
