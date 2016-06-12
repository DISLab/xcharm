#ifndef __Vertex_h__
#define __Vertex_h__
#include "DataTypes.h"
#include "NDMeshStreamer.h"
#include "Graph.decl.h"
#include "Transport.h"

namespace GraphLib {
	/**
	 * Edge class; user edge class must be derived from this class.
	 **/
	struct Edge {
		// Adjacent vertex id
		VertexId v;
		Edge(VertexId v) : v(v) {}
	};

	/**
	 * Vertex template class; user vertex class must be derived from this class.
	 **/
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

				// adds edge to the vertex
				void addEdge(const E & e) { 
					edges.push_back(e); 
				}

				// TODO: remove this methid (duplication of addEdge) 
				void connectVertex(const E & e) { 
					edges.push_back(e); 
				}

				// returns edge number
				inline const int getEdgeNumber() { return edges.size(); }

				// sends message to the specified vertex (not necessary to neighbour)
				template <typename M> void sendMessage(M & m, VertexId v) {
					transport.sendMessage(m, v);
				}

				// sends message to the specified vertex (not necessary to neighbour)
				template <typename M> void sendMessage(/*const M &*/M m, VertexId v) {
					transport.sendMessage(m, v);
				}

				// receives message and call user process method (only for Charm transport)
				template <typename M> void recv(const M & m) {
					static_cast<V *>(this)->process(m);
				}
		};

// vertex adn edge iterators
#define FORALL_ADJ_EDGES(e, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (e = *i, true) : false; i++)
#define FORALL_ADJ_VERTICES(v, V) \
for(std::vector<V::Edge>::iterator i = edges.begin(); \
		i != edges.end() ? (v = i->v, true) : false; i++)
//TODO: add more iterators

}


#endif
