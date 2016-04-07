#include "GraphGenerator.decl.h"
#include <graph500/graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>

template <class Graph, class Edge, class Opts>
	class GraphGenerator : public CBase_GraphGenerator<Graph, Edge, Opts> {
		Graph g;
		Opts opts;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), opts(opts) {}
			void generate() {

				// graph generatoroint 
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

				boost::graph500_iterator graph500iter(opts.scale, e_start, a, b);

				boost::timer::cpu_times tstart, tend;
				boost::timer::cpu_timer timer;

				//if (process_id(pg) == 0) {

				tstart.clear();
				tstart = timer.elapsed();
				//}

				int thrash = 0;
				uint64_t ecounter = 0;

				//std::cout << process_id(pg) << ": " << e_start << "," << e_count << "\n";

				for (uint64_t i = 0; i < /*m/num_processes(pg)*/ e_count; i++) {

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

					double w = drand48();
#if defined(UCHARELIB)
					g[e.first]->connectVertex(Edge(e.second, w));
					g[e.second]->connectVertex(Edge(e.first, w));
#else
					g[e.first].connectVertex(Edge(e.second, w));
					g[e.second].connectVertex(Edge(e.first, w));
#endif
					graph500iter.increment();

					ecounter++;
				} 
			} 
	};

#define CK_TEMPLATES_ONLY
#include "GraphGenerator.def.h"
#undef CK_TEMPLATES_ONLY
