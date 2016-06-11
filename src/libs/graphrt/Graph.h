#ifndef __Graph_h__ 
#define __Graph_h__
#include "DataTypes.h"
#include "NDMeshStreamer.h"
#include "Graph.decl.h"
#include "Transport.h"

namespace GraphLib {
	struct Edge {
		VertexId v;
	};

	template <typename V, typename E, TransportType transportType>
		class Vertex :  public CBase_Vertex<V, E, transportType>	{
			public:
				//typedef V Vertex; 
				typedef E Edge; 
				typedef CProxy_Vertex<V, E, transportType> CProxy_V;
				typedef CkIndex_Vertex<V, E, transportType> CkIndex_V;
				template <typename dtype>
					using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, int, V, SimpleMeshRouter>; 

			protected:
				std::vector<E> edges;
				Transport<V, CProxy_V, transportType> transport;

			public:
				Vertex() : transport(this->thisProxy) {
					__register();
				}
				Vertex(CkMigrateMessage *m) {}

				void __register() {
					CkIndex_V::template idx_init_marshall4<int>();
					CkIndex_V::template idx_init_marshall4<long>();
					CkIndex_V::template idx_init_marshall4<double>();
					CkIndex_V::template idx_init_marshall4<char>();
				}

				// only for Tram transport
				template <typename M> void init(const CProxy_Aggregator<M> & aggregator) {
					transport.init(aggregator);
				}

				void connectVertex(const E & e) { 
					edges.push_back(e); 
				}
				void addEdge(const E & e) { 
					edges.push_back(e); 
				}
				inline const int getEdgeNumber() { return edges.size(); }
				template <typename M> void sendMessage(M & m, VertexId v) {
					transport.sendMessage(m, v);
				}
				template <typename M> void sendMessage(/*const M &*/M m, VertexId v) {
					transport.sendMessage(m, v);
				}
				template <typename M> void recv(const M & m) {
					//transport.recv(m);
					static_cast<V *>(this)->process(m);
				}
				void foo() {CkPrintf("Foooo\n");}
		};

#define FORALL_ADJ_EDGES(e, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (e = *i, true) : false; i++)
#define FORALL_ADJ_VERTICES(v, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (v = i->v, true) : false; i++)

	template <typename V, typename E, typename CProxy_Vertex, TransportType transportType>
		class Graph;

	template <typename V, typename E, typename CProxy_Vertex>
		class Graph<V, E, CProxy_Vertex, TransportType::Charm> {
			public:
				typedef V Vertex;
				typedef E Edge;
				typedef CProxy_Vertex Proxy;
			private:
				CProxy_Vertex g;
			public:
				Graph() {}
				Graph(CmiUInt8 N) {
					g = CProxy_Vertex::ckNew(N);
				}
				Graph(CProxy_Vertex g) : g(g) {}
				CProxy_Vertex & getProxy() { return g; }
				void pup(PUP::er & p) {
					p | g;
				}
		};

	template <typename V, typename E, typename CProxy_Vertex>
		class Graph<V, E, CProxy_Vertex, TransportType::Tram> {
			public:
				typedef V Vertex;
				typedef E Edge;
				typedef CProxy_Vertex Proxy;
				template <typename dtype>
					using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, int, V, SimpleMeshRouter>; 
			private:
				CProxy_Vertex graphProxy;
			public:
				Graph() {}
				Graph(CmiUInt8 N) {
					graphProxy = CProxy_Vertex::ckNew(N);

					// register all possible aggregators
					registerAggregator<int>();
					registerAggregator<long>();
					registerAggregator<double>();
					registerAggregator<char>();
					//..

					//FIXME::
				}
				Graph(CProxy_Vertex graphProxy) : graphProxy(graphProxy) {}
				CProxy_Vertex & getProxy() { return graphProxy; }
				void pup(PUP::er & p) {
					p | graphProxy;
				}
				template <typename M> CProxy_Aggregator<M> & getAggregator() {
					static CProxy_Aggregator<M> aggregator;
					return aggregator;
				}
				template <typename M> void registerAggregator() {
					const int numMsgsBuffered = 1024;
					int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
					CProxy_Aggregator<M> &aggregator = getAggregator<M>();
					aggregator = CProxy_Aggregator<M>::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1); 
					graphProxy.init(aggregator);
					aggregator.init(-1);
				}
		};
}

#define CK_TEMPLATES_ONLY
#include "Graph.def.h"
#undef CK_TEMPLATES_ONLY

#endif
