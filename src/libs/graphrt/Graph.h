#ifndef __Graph_h__ 
#define __Graph_h__
#include "DataTypes.h"
#include "Graph.decl.h"

namespace GraphLib {
	typedef CmiUInt8 VertexId;
	struct Edge {
		VertexId v;
	};

	template <typename V, typename E, TransportType transportType>
		class Vertex;

	template <typename V, typename E>
		class Vertex<V, E, TransportType::Charm> : public CBase_Vertex<V, E, TransportType::Charm>	{
			public:
				//typedef V Vertex; 
				typedef E Edge; 

			protected:
				std::vector<E> edges;

			public:
				Vertex() {}
				Vertex(CkMigrateMessage *m) {}
				void connectVertex(const E & e) { 
					edges.push_back(e); 
				}
				void addEdge(const E & e) { 
					edges.push_back(e); 
				}
				inline const int getEdgeNumber() { return edges.size(); }
				template <typename M> void sendMessage(M & m, VertexId v) {
					this->thisProxy[v].recv(m);
				}
				template <typename M> void sendMessage(const M & m, VertexId v) {
					this->thisProxy[v].recv(m);
				}
				template <typename M> void recv(const M & m) {
					static_cast<V *>(this)->process(m);
				}
		};

#define FORALL_ADJ_EDGES(e, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (e = *i, true) : false; i++)
#define FORALL_ADJ_VERTICES(v, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (v = i->v, true) : false; i++)

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
#include "Graph.def.h"
#undef CK_TEMPLATES_ONLY

#endif
