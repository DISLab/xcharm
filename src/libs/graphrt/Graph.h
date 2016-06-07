#ifndef __Graph_h__ 
#define __Graph_h__
#include "GraphLib.decl.h"

namespace GraphLib {
	template <typename E>
		class Vertex : public CBase_Vertex {
			private:
				std::vector<E> edges;
			public:
				Vertex() {}
				void addEdge(const E & e) { edges.push_back(e); }
		};

	template <typename V, typename E, typename CProxy_Vertex>
		class Graph {
			public:
				typedef V Vertex;
				typedef E Edge;
				typedef CProxy_Vertex Proxy;
			private:
				CProxy_Vertex g;
			public:
				Graph() {}
				Graph(CProxy_Vertex g) : g(g) {}
				CProxy_Vertex & getProxy() { return g; }
				void pup(PUP::er & p) {
					p | g;
				}
		};
}

#define CK_TEMPLATES_ONLY
#include "GraphLib.def.h"
#undef CK_TEMPLATES_ONLY

#endif
