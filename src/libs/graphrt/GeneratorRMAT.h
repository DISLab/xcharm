#ifndef __GeneratorRMAT_h_
#define __GeneratorRMAT_h_

namespace GraphLib {

	template <class Graph, class Opts, VertexMapping vertexMapping>
		class RMAT_Generator_Charm;

	template <class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator>
		class RMAT_Generator_Tram;

	template <class Graph, class Opts>
		class RMAT_Generator;

	template <class Graph, class Opts>
		class RMAT_Generator_Charm<Graph, Opts, VertexMapping::SingleVertex> : 
		public CBase_RMAT_Generator_Charm <Graph, Opts, VertexMapping::SingleVertex>,
				RMAT_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				public:
					RMAT_Generator_Charm(Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()), 
						RMAT_Generator<Graph, Opts>(g, opts) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						RMAT_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();
						graphProxy[e.first].connectVertex(Edge(e.second, w));
						//graphProxy[e.second].connectVertex(Edge(e.first, w));
					}
			};

	template <class Graph, class Opts>
		class RMAT_Generator_Charm<Graph, Opts, VertexMapping::MultiVertex> : 
		public CBase_RMAT_Generator_Charm <Graph, Opts, VertexMapping::MultiVertex>,
				RMAT_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				public:
					RMAT_Generator_Charm(Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()), 
						RMAT_Generator<Graph, Opts>(g, opts) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						RMAT_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();
						//graphProxy[e.first / (opts.N / CmiNumPes())].connectVertex(Edge(e.second, w));
						graphProxy[Graph::base(e.first)].connectVertex(std::make_pair(e.first, Edge(e.second, w)));
						//graphProxy[e.second].connectVertex(Edge(e.first, w));
					}
			};


	template <class Graph, class Opts, class CProxy_Aggregator>
		class RMAT_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, CProxy_Aggregator> :
			public CBase_RMAT_Generator_Tram <Graph, Opts, VertexMapping::SingleVertex, CProxy_Aggregator>,
				RMAT_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Aggregator aggregator;
				public:
					RMAT_Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : g(g), aggregator(aggregator), 
						RMAT_Generator<Graph, Opts>(g, opts) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						RMAT_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();

						//CkPrintf("adding edge (%lld, %lld)\n", e.first, e.second);
						ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter>
							* localAggregator = aggregator.ckLocalBranch();
						localAggregator->insertData(Edge(e.second, w), e.first);
						//localAggregator->insertData(Edge(e.first, w), e.second);
					}
			};

	template <class Graph, class Opts, class CProxy_Aggregator>
		class RMAT_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, CProxy_Aggregator> :
			public CBase_RMAT_Generator_Tram <Graph, Opts, VertexMapping::MultiVertex, CProxy_Aggregator>,
				RMAT_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Aggregator aggregator;
				public:
					RMAT_Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : g(g), aggregator(aggregator), 
						RMAT_Generator<Graph, Opts>(g, opts) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						RMAT_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(std::pair<uint64_t,uint64_t> & e) {
						double w = drand48();

						//CkPrintf("adding edge (%lld, %lld)\n", e.first, e.second);
						ArrayMeshStreamer<std::pair<CmiUInt8, Edge>, long long, Vertex, SimpleMeshRouter>
							* localAggregator = aggregator.ckLocalBranch();
						localAggregator->insertData(std::make_pair(e.first, Edge(e.second, w)), Graph::base(e.first));
						//localAggregator->insertData(Edge(e.first, w), e.second);
					}
			};

	template <class Graph, class Opts>
		class RMAT_Generator {
			protected:
				Graph g;
				Opts opts;
			public:
				RMAT_Generator (Graph & g, Opts & opts) : g(g), opts(opts) {}
				virtual void addEdge(std::pair<uint64_t,uint64_t> & e) = 0;
				void do_generate();
		};

	template <class Graph, class Opts>
		void RMAT_Generator<Graph, Opts>::do_generate() {
			uint64_t seed64 = 12345;
			boost::rand48 gen, synch_gen;
			gen.seed(seed64);
			synch_gen.seed(seed64);
			srand48(12345);

			boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
			uint64_t a = rand_64(gen);
			uint64_t b = rand_64(gen);

			uint64_t e_start = CkMyPe() * opts.M / CkNumPes(); 
				//world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
			uint64_t e_count = opts.M / CkNumPes(); 
				//(std::min)((m + world.size() - 1) / world.size(), m - e_start);

			boost::graph500_iterator graph500iter(
					opts.strongscale == true ? opts.scale : opts.scale + __log2p2(CkNumPes()), 
					e_start, a, b);

			boost::timer::cpu_times tstart, tend;
			boost::timer::cpu_timer timer;

			tstart.clear();
			tstart = timer.elapsed();

			int thrash = 0;
			uint64_t ecounter = 0;

			for (uint64_t i = 0; i < e_count; i++) {
				if (((double)i/e_count)*100 > thrash) {
					//synchronize(pg);
					if (CkMyPe() == 0) {
						tend = timer.elapsed();
						CkPrintf("%d%%, %lld edges added, %0.2f sec\n", thrash, 
								ecounter, (double)(tend.wall - tstart.wall)/1000000000);
					}
					thrash+=10;
				}
				std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
				//add_edge(vertex(e.first, g), vertex(e.second, g), g);

				addEdge(e);

				graph500iter.increment();

				if (ecounter % YIELD_THRASHOLD)
					CthYield();
				ecounter++;
			}
		}

	/**
	 * Kronecker graph generator
	 **/
	template <class Graph, class Opts>
		class Generator<Graph, Opts, GraphGeneratorType::Kronecker> {
			protected:
				Graph g;
				Opts opts;
			public:
				Generator (Graph & g, Opts & opts) : g(g), opts(opts) {}
				virtual void addEdge(std::pair<uint64_t,uint64_t> & e) = 0;
				void do_generate();
		};

	template <class Graph, class Opts>
		void Generator<Graph, Opts, GraphGeneratorType::Kronecker>::do_generate() {
			uint64_t seed64 = 12345;
			boost::rand48 gen, synch_gen;
			gen.seed(seed64);
			synch_gen.seed(seed64);
			srand48(12345);

			boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
			uint64_t a = rand_64(gen);
			uint64_t b = rand_64(gen);

			uint64_t e_start = CkMyPe() * opts.M / CkNumPes(); 
				//world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
			uint64_t e_count = opts.M / CkNumPes(); 
				//(std::min)((m + world.size() - 1) / world.size(), m - e_start);

			boost::graph500_iterator graph500iter(
					opts.strongscale == true ? opts.scale : opts.scale + __log2p2(CkNumPes()), 
					e_start, a, b);

			boost::timer::cpu_times tstart, tend;
			boost::timer::cpu_timer timer;

			tstart.clear();
			tstart = timer.elapsed();

			int thrash = 0;
			uint64_t ecounter = 0;

			for (uint64_t i = 0; i < e_count; i++) {
				if (((double)i/e_count)*100 > thrash) {
					//synchronize(pg);
					if (CkMyPe() == 0) {
						tend = timer.elapsed();
						CkPrintf("%d%%, %lld edges added, %0.2f sec\n", thrash, 
								ecounter, (double)(tend.wall - tstart.wall)/1000000000);
					}
					thrash+=10;
				}
				std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
				//add_edge(vertex(e.first, g), vertex(e.second, g), g);

				addEdge(e);

				graph500iter.increment();

				if (ecounter % YIELD_THRASHOLD)
					CthYield();
				ecounter++;
			}
		}
}

#endif

