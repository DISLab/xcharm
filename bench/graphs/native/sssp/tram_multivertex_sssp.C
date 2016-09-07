#include <GraphLib.h>
#include <common.h>
#include <deque>

typedef struct __dtype {
	CmiUInt8 v;
	double weight;
	CmiUInt8 parent;
	__dtype() {}
	__dtype(CmiUInt8 v, double weight, CmiUInt8 parent) : v(v), weight(weight), parent(parent) {}
	void pup(PUP::er & p) { p | v; p | weight; p | parent; }
} dtype;

class SSSPMultiVertex;
class SSSPEdge;
class	SSSPGraph;

#include "tram_multivertex_sssp.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, SSSPMultiVertex,
                         SimpleMeshRouter> aggregator;
const int numMsgsBuffered = 1024;

class SSSPGraph : public GraphLib::Graph<
	SSSPMultiVertex,
	SSSPEdge,
	CProxy_SSSPMultiVertex,
	GraphLib::TransportType::Charm> {
public:
	SSSPGraph() : 
		GraphLib::Graph<
				SSSPMultiVertex, 
				SSSPEdge,
				CProxy_SSSPMultiVertex, 
				GraphLib::TransportType::Charm >()
		{}
	SSSPGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				SSSPMultiVertex, 
				SSSPEdge,
				CProxy_SSSPMultiVertex, 
				GraphLib::TransportType::Charm >(CmiNumPes())	{

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, long long, SSSPMultiVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, g, 1);
	}
	void start(CmiUInt8 root) {
		CkCallback startCb(CkIndex_SSSPMultiVertex::foo(), g[root / (N / CmiNumPes())]);
		CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
		aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
		CkStartQD(CkCallbackResumeThread());
		g[root / (N / CmiNumPes())].make_root(root);
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		CkCallback startCb(CkIndex_SSSPMultiVertex::foo(), g[root / (N / CmiNumPes())]);
		CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
		aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
		CkStartQD(CkCallbackResumeThread());
		g[root / (N / CmiNumPes())].make_root(root);
		CkStartQD(cb);
	}
	void getScannedVertexNum() {
		g.getScannedVertexNum();
	}
	void verify() {
		g.verify();
	}
};

struct SSSPEdge {
	CmiUInt8 v;
	double w;

	SSSPEdge() {}
	SSSPEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w; 
	}
};

class SSSPVertex {
	private:
		CmiUInt8 thisIndex;
		std::vector<SSSPEdge> adjlist;
		double weight;
		CmiUInt8 parent;

	public:
		SSSPVertex() : 
			thisIndex(-1), parent(-1),
			weight(std::numeric_limits<double>::max())  {}
		SSSPVertex(CmiUInt8 idx) : 
			thisIndex(idx), parent(-1),
			weight(std::numeric_limits<double>::max())  {}
		void connectVertex(const SSSPEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(SSSPMultiVertex & multiVertex, const double & weight, const CmiUInt8 & parent); 
		void verify(SSSPMultiVertex & multiVertex); 
		void check(const double & weight); 
		const CmiUInt8 getScannedVertexNum() { return (parent != -1 ? 1 : 0); }
};

class SSSPMultiVertex : public CBase_SSSPMultiVertex {
	private:
		std::vector<SSSPVertex> vertices;
		std::deque<dtype> q;

	public:
		SSSPMultiVertex() {
			//vertices.assign(N / ckGetArraySize(), SSSPVertex());
			for(CmiUInt8 i = 0; i < N / ckGetArraySize(); i++)
				vertices.push_back(SSSPVertex(getBase() + i));
		}
		/*SSSPMultiVertex(CmiUInt8 n) {
			vertices.assign(n, SSSPVertex());
		}*/

		void connectVertex(const std::pair<CmiUInt8, SSSPEdge> & edge) {
			vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
		}

		void process(const std::pair<CmiUInt8, SSSPEdge > & edge) {
			connectVertex(edge);
		}

		SSSPMultiVertex(CkMigrateMessage *msg) {}

		inline std::deque<dtype> & getQ() { return q; }

		void make_root(const CmiUInt8 & root) {
			CkAssert(getBaseIndex(root) == thisIndex);
			process(dtype(root, 0, root));
		}

		inline void process(const dtype & data) {

			q.push_back(data);
			while (!q.empty()) {
				dtype d = q.front();
				CkAssert(getBaseIndex(d.v) == thisIndex);

				q.pop_front();
				vertices[getLocalIndex(d.v)].update(*this, d.weight, d.parent);
			}
		}

		void verify() {
			typedef std::vector<SSSPVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				dtype data = q.front();
				CkAssert(getBaseIndex(data.v) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(data.v)].check(data.weight);
			}
		}

		void check(const CmiUInt8 & v, int level) {
			vertices[getLocalIndex(v)].check(level);
		}

		void getScannedVertexNum() {
			CmiUInt8 numScannedVertices = 0;
			typedef std::vector<SSSPVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				numScannedVertices += it->getScannedVertexNum();

			contribute(sizeof(CmiUInt8), &numScannedVertices, CkReduction::sum_long,
								 CkCallback(CkReductionTarget(TestDriver, done),
														driverProxy));
		}

		inline bool isLocalIndex(const CmiUInt8 & gIdx) { 
			return thisIndex == (gIdx / (N / ckGetArraySize())); 
		}
		inline CmiUInt8 getBaseIndex(const CmiUInt8 & gIdx) { 
			return gIdx / (N / ckGetArraySize()); 
		}
		inline CmiUInt8 getLocalIndex(const CmiUInt8 & gIdx) { 
			return gIdx % (N / ckGetArraySize()); 
		}
		inline CmiUInt8 getBase() { 
			return thisIndex * (N / ckGetArraySize()); 
		}

		void foo() {} 
};

void SSSPVertex::update(SSSPMultiVertex & multiVertex, const double & weight, const CmiUInt8 & parent) {
	if (weight < this->weight) {
		//CkPrintf("%lld: update %e -> %e\n", thisIndex, this->weight, weight);
		std::deque<dtype> & q = multiVertex.getQ();
		CProxy_SSSPMultiVertex & thisProxy = multiVertex.thisProxy; 
		this->weight = weight;
		this->parent = parent;

		ArrayMeshStreamer<dtype, long long, SSSPMultiVertex, SimpleMeshRouter>
			* localAggregator = aggregator.ckLocalBranch();
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			if (multiVertex.isLocalIndex(it->v))
				q.push_back(dtype(it->v, this->weight + it->w, thisIndex));
			else
				//thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v, this->weight + it->w, thisIndex);
				localAggregator->insertData(dtype(it->v, this->weight + it->w, thisIndex), multiVertex.getBaseIndex(it->v));
		}
	}
}

void SSSPVertex::verify(SSSPMultiVertex & multiVertex) {
	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_SSSPMultiVertex & thisProxy = multiVertex.thisProxy; 

	if ((parent != -1) && (parent != thisIndex)) {
		if (multiVertex.isLocalIndex(parent))
			q.push_back(dtype(parent, weight, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(parent)].check(parent, weight);
	}
}

void SSSPVertex::check(const double & weight) {
	CkAssert(this->weight < weight);
}

typedef GraphLib::GraphGenerator<
	SSSPGraph, 
	Options, 
	GraphLib::VertexMapping::MultiVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"
#include "tram_multivertex_sssp.def.h"
