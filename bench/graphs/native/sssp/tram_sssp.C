//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
#include <GraphLib.h>
#include <common.h>
#include <sstream>

class SSSPVertex;
class SSSPEdge;
class	SSSPGraph;

#define RADIX_USED
//#define RADIX RADIX

typedef struct __dtype {
	CmiUInt8 v;
	double w;
#if defined (RADIX_USED)
	int r;
	__dtype (CmiUInt8 v, double w, int r) : v(v), w(w), r(r) {}
#else
	__dtype (CmiUInt8 v, double w) : v(v), w(w) {}
#endif
	__dtype() {}
	void pup(PUP::er & p) { 
		p | v;
		p | w;
#if defined (RADIX_USED)
		p | r;
#endif
	}
} dtype;
#include "tram_sssp.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, SSSPVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

class SSSPGraph : public GraphLib::Graph<
	SSSPVertex,
	SSSPEdge,
	CProxy_SSSPVertex,
	GraphLib::TransportType::Charm> {

public:
	SSSPGraph() : 
		GraphLib::Graph<
				SSSPVertex, 
				SSSPEdge,
				CProxy_SSSPVertex, 
				GraphLib::TransportType::Charm >()	
		{}

	SSSPGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				SSSPVertex, 
				SSSPEdge,
				CProxy_SSSPVertex, 
				GraphLib::TransportType::Charm >(nVertex) {
		int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
		CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);
		// Instantiate communication library group with a handle to the client
		aggregator =
			CProxy_ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
			::ckNew(numMsgsBuffered, 2, dims, g, 1);
	}
	void start(CmiUInt8 root) {
    CkCallback startCb(CkIndex_SSSPVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
    CkCallback startCb(CkIndex_SSSPVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
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

class SSSPVertex : public CBase_SSSPVertex {
private:
	std::vector<SSSPEdge> adjlist;
	double weight;
	CmiUInt8 parent;
	CmiUInt8 totalUpdates;

public:
  SSSPVertex() : weight(std::numeric_limits<double>::max()), parent(-1), totalUpdates(0) {

    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const SSSPEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const SSSPEdge & edge) {
		connectVertex(edge);
	}

  SSSPVertex(CkMigrateMessage *msg) {}

	void make_root() {
		weight = 0;
		parent = thisIndex;
    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();
		// broadcast
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIX_USED)
			localAggregator->insertData(dtype(thisIndex, weight + it->w, RADIX), it->v);
			//thisProxy[it->v].update(dtype(thisIndex, weight + it->w, RADIX));
#else
			localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
			//thisProxy[it->v].update(dtype(thisIndex, weight + it->w));
#endif
		}
	}

	inline void process(const dtype & m) {
		const CmiUInt8 & v = m.v;
		const double & w = m.w;
#if defined (RADIX_USED)
		const int & r = m.r;
#endif

		//CkPrintf("%d: process called\n", thisIndex);

    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		if (w < weight) {
			totalUpdates++;

			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIX_USED)
				if (r > 0 )
					localAggregator->insertData(dtype(thisIndex, weight + it->w, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(thisIndex, weight + it->w, RADIX));
#else
				localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
				//thisProxy[it->v].update(dtype(thisIndex, weight + it->w));
#endif
			}
		}
  }

	void update(const dtype & m) {
		const CmiUInt8 & v = m.v;
		const double & w = m.w;
#if defined (RADIX_USED)
		const int & r = m.r;
#endif
    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		//CkPrintf("%d: update called\n", thisIndex);

		if (w < weight) {
			totalUpdates++;

		//CkPrintf("%d: %2.2f -> %2.2f\n", thisIndex, weight, w);
			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIX_USED)
				if (r > 0 )
					localAggregator->insertData(dtype(thisIndex, weight + it->w, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(thisIndex, weight + it->w, RADIX));
#else
				localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
				//thisProxy[it->v].update(dtype(thisIndex, weight + it->w));
#endif
			}
		}
  }

	void getScannedVertexNum() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);	
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, 
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void countTotalUpdates(const CkCallback & cb) {
		contribute(sizeof(totalUpdates), &totalUpdates, CkReduction::sum_long, cb);
	}

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent].check(weight);
	}

	void check(double w) {
		CkAssert(weight < w);
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %.2f, %lld, %s\n", thisIndex, weight, parent, ss.str().c_str());
	}
};

typedef GraphLib::GraphGenerator<
	SSSPGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "tram_sssp.def.h"
