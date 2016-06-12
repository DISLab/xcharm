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

	/**
	 * Graph template class
	 **/
	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
		class Generator;

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
