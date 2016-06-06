#include <iostream>
#include <graph500/graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>
#include "NDMeshStreamer.h"
#include "GraphGenerator.decl.h"

#define YIELD_THRASHOLD	1024

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}

template <class Graph, class Opts>
class Generator {
	protected:
		Graph g;
		Opts opts;
	public:
		Generator (Graph & g, Opts & opts) : g(g), opts(opts) {}
		virtual void addEdge(std::pair<uint64_t,uint64_t> & e) = 0;
		void do_generate() {
			uint64_t seed64 = 12345;
			boost::rand48 gen, synch_gen;
			gen.seed(seed64);
			synch_gen.seed(seed64);
			srand48(12345);

			boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
			uint64_t a = rand_64(gen);
			uint64_t b = rand_64(gen);

			uint64_t e_start = CkMyPe() * opts.M / CkNumPes(); //world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
			uint64_t e_count = opts.M / CkNumPes(); //(std::min)((m + world.size() - 1) / world.size(), m - e_start);

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
}; 

template <class Graph, class Edge, class Opts>
	class GraphGenerator : public CBase_GraphGenerator<Graph, Edge, Opts>, public Generator<Graph, Opts> {
		Graph g;
		Opts opts;
		public:
			GraphGenerator (Graph & g, Opts & opts) : 
				g(g), opts(opts), Generator<Graph, Opts>(g,opts) {
			}
			void addEdge(std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
#if defined(UCHARELIB)
					g[e.first]->connectVertex(Edge(e.second, w));
					g[e.second]->connectVertex(Edge(e.first, w));
#else
					g[e.first].connectVertex(Edge(e.second, w));
					g[e.second].connectVertex(Edge(e.first, w));
#endif
			}

			void generate() { Generator<Graph, Opts>::do_generate(); }
			//FIXME:
			void _sdag_pup(PUP::er & p) {}
	};

template <class GraphProxy, class Vertex, class Edge, class Opts, class CProxy_Aggregator>
	class GraphGenerator_Aggregator : 
		public CBase_GraphGenerator_Aggregator<GraphProxy, Vertex, Edge, 
			Opts, CProxy_ArrayMeshStreamer<Edge, int, Vertex, SimpleMeshRouter> >, 
		public Generator<GraphProxy, Opts> {
			CProxy_Aggregator aggregator;
		public:
			GraphGenerator_Aggregator (GraphProxy & gp, Opts & opts, CProxy_Aggregator & aggregator) : 
				aggregator(aggregator), Generator<GraphProxy, Opts>(gp, opts) {
			}
			void addEdge(std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					ArrayMeshStreamer<Edge, int, Vertex, SimpleMeshRouter>
						* localAggregator = aggregator.ckLocalBranch();
					localAggregator->insertData(Edge(e.second, w), e.first);
					localAggregator->insertData(Edge(e.first, w), e.second);
			}
			void generate() { Generator<GraphProxy, Opts>::do_generate(); }
	};

// Aggregator types
typedef enum {NoAggregator = 0, TramAggregator} AggregatorType; 

// Graph generator manager class
template <class Graph, class Opts, AggregatorType aggrType = NoAggregator >
 class GraphGeneratorMgr;

template <class Graph, class Opts>
 class GraphGeneratorMgr<Graph, Opts, NoAggregator> {
	 typedef typename Graph::Proxy GraphProxy;
	 typedef typename Graph::Vertex Vertex;
	 typedef typename Graph::Edge Edge;
	 CProxy_GraphGenerator<GraphProxy, Edge, Opts> generator; 
	 public:
	 	GraphGeneratorMgr(GraphProxy gp, Opts & opts) {
			generator = CProxy_GraphGenerator<GraphProxy, Edge, Opts>::ckNew(gp,opts);
		}
		void generate() { generator.generate(); }
 };

template <class Graph, class Opts>
 class GraphGeneratorMgr<Graph, Opts, TramAggregator> {
	 typedef typename Graph::Proxy GraphProxy;
	 typedef typename Graph::Vertex Vertex;
	 typedef typename Graph::Edge Edge;
	 typedef CProxy_ArrayMeshStreamer<Edge, int, Vertex, SimpleMeshRouter> CProxy_Aggregator;
	 typedef CProxy_GraphGenerator_Aggregator<GraphProxy, Vertex, Edge, Opts, CProxy_Aggregator> CProxy_Generator;
	 typedef CkIndex_ArrayMeshStreamer<Edge, int, Vertex, SimpleMeshRouter> CkIndex_Aggregator;
	 typedef CkIndex_GraphGenerator_Aggregator<GraphProxy, Vertex, Edge, Opts, CProxy_Aggregator> CkIndex_Generator;
	 CProxy_Generator generator; 
	 CProxy_Aggregator aggregator;
	 GraphProxy gp;
	 Opts opts;
	 public:
		 GraphGeneratorMgr(GraphProxy & gp, Opts & opts) : gp(gp), opts(opts) {
			 const int numMsgsBuffered = 1024;
			 int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
			 aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, gp, 1);
			 generator = CProxy_Generator::ckNew(gp, opts, aggregator);
		 }
		 void generate() {
			 CkCallback startCb(CkIndex_Generator::generate(), generator), fooCb;
			 aggregator.init(gp.ckGetArrayID(), startCb, fooCb, -1, true);
		 }
 };

#define CK_TEMPLATES_ONLY
#include "GraphGenerator.def.h"
#undef CK_TEMPLATES_ONLY

