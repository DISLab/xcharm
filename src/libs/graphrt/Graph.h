#ifndef __Graph_h__ 
#define __Graph_h__
#include "DataTypes.h"
#include "NDMeshStreamer.h"
#include "Graph.decl.h"
#include "Vertex.h"
#include "Transport.h"

namespace GraphLib {

	/**
	 * Graph template class
	 **/
	template <typename V, typename E, typename CProxy_Vertex, TransportType transportType>
		class Graph;

	/**
	 * Partial Graph template specialization for Charm transport
	 **/
	template <typename V, typename E, typename CProxy_Vertex>
		class Graph<V, E, CProxy_Vertex, TransportType::Charm> {
			public:
				typedef V Vertex;
				typedef E Edge;
				typedef CProxy_Vertex Proxy;
			protected:
				CProxy_Vertex g;
			public:
				Graph() {}
				Graph(CmiUInt8 N) {
					// create chare array for Graph vertices
					g = CProxy_Vertex::ckNew(N);
				}
				Graph(CProxy_Vertex g) : g(g) {}
				CProxy_Vertex & getProxy() { return g; }
				void pup(PUP::er & p) {
					p | g;
				}
		};

	/**
	 * Partial Graph template specialization for Tram transport
	 **/
	template <typename V, typename E, typename CProxy_Vertex>
		class Graph<V, E, CProxy_Vertex, TransportType::Tram> {
			public:
				typedef V Vertex;
				typedef E Edge;
				typedef CProxy_Vertex Proxy;
				template <typename dtype>
					using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, long long, V, SimpleMeshRouter>; 
			private:
				CProxy_Vertex graphProxy;
			public:
				Graph() {}
				Graph(CmiUInt8 N) {
					// create chare array for Graph vertices
					graphProxy = CProxy_Vertex::ckNew(N);

					// register all possible aggregators
					registerAggregator<int>();
					registerAggregator<long>();
					registerAggregator<double>();
					registerAggregator<char>();
					//..
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
