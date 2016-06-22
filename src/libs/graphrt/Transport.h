#ifndef __Transport_h__ 
#define __Transport_h__
#include "DataTypes.h"
#include "NDMeshStreamer.h"

namespace GraphLib {

	/**
	 * Transport template class
	 **/
	template <typename V, typename CProxy_V, TransportType transportType>
		class Transport;

	/**
	 * Charm transport
	 **/
	template <typename V, typename CProxy_V>
	class Transport<V, CProxy_V, TransportType::Charm> {
		private:
			CProxy_V thisProxy;
		public:
			Transport() {}
			Transport(CProxy_V & thisProxy) : thisProxy(thisProxy) {}
			template <typename M> void sendMessage(M & m, VertexId v) {
				this->thisProxy[v].recv(m);
			}

			// FIXME: this is needed for compiling (remove it later)
			template <typename dtype>
				using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, long long, V, SimpleMeshRouter>; 
			template <typename M> void init(const CProxy_Aggregator<M> & aggregator) {}
	};

	/**
	 * Tram transport
	 **/
	template <typename V, typename CProxy_V>
	class Transport<V, CProxy_V, TransportType::Tram> {
		private:
			CProxy_V thisProxy;
		public:
			Transport() {}
			Transport(CProxy_V & thisProxy) : thisProxy(thisProxy) {}
			template <typename dtype>
				using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, long long, V, SimpleMeshRouter>; 
			template <typename M> void init(const CProxy_Aggregator<M> & aggregator) {
				getAggregator<M>() = aggregator;
			}
			template <typename M> void sendMessage(M & m, VertexId v) {
				ArrayMeshStreamer<M, long long, V, SimpleMeshRouter>
					* localAggregator = getAggregator<M>().ckLocalBranch();
				localAggregator->insertData(m, v);
			}
			template <typename M> CProxy_Aggregator<M> & getAggregator() {
				static CProxy_Aggregator<M> aggregator;
					return aggregator;
			}
	};

	// FIXME: this is just for compilation!
	template <typename V, typename CProxy_V>
	class Transport<V, CProxy_V, TransportType::uChareLib> {
		private:
			CProxy_V thisProxy;
		public:
			Transport() {}
			Transport(CProxy_V & thisProxy) : thisProxy(thisProxy) {}
			template <typename dtype>
				using CProxy_Aggregator = CProxy_ArrayMeshStreamer<dtype, long long, V, SimpleMeshRouter>; 
			template <typename M> void init(const CProxy_Aggregator<M> & aggregator) {
			}
	};

}
#endif
