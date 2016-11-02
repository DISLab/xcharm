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
	//template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
	//	class Generator;

	template <class Graph, class Opts, GraphGeneratorType graphGeneratorType>
		class Generator {
			protected:
				Graph g;
				Opts opts;
			public:
				Generator (Graph & g, Opts & opts) : g(g), opts(opts) {}
				virtual void addEdge(std::pair<uint64_t,uint64_t> & e) = 0;
				void do_generate() {}
		};

}

#include "GeneratorRMAT.h"
#include "GeneratorSSCA.h"
#include "GeneratorRandom.h"

#endif
