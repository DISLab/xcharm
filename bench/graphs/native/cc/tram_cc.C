#include <GraphLib.h>
#include <common.h>

class CCVertex;
class CCEdge;
class	CCGraph;

#define RADIX_ENABLED
//#define RADIX 1

typedef struct __dtype {
	CmiUInt8 c; 
	int r;
  __dtype(CmiUInt8 c, int r) : 
		c(c), r(r) {}
	__dtype() {}
	void pup(PUP::er & p) {
		p | c;
		p | r;
	}
} dtype;

#include "tram_cc.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, CCVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

class CCGraph : public GraphLib::Graph<
	CCVertex,
	CCEdge,
	CProxy_CCVertex,
	GraphLib::TransportType::Charm> {

public:
	CCGraph() : 
		GraphLib::Graph<
				CCVertex, 
				CCEdge,
				CProxy_CCVertex, 
				GraphLib::TransportType::Charm >()	
		{}

	CCGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				CCVertex, 
				CCEdge,
				CProxy_CCVertex, 
				GraphLib::TransportType::Charm >(nVertex) {
		int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
		CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);
		// Instantiate communication library group with a handle to the client
		aggregator =
			CProxy_ArrayMeshStreamer<dtype, long long, CCVertex, SimpleMeshRouter>
			::ckNew(numMsgsBuffered, 2, dims, g, 1);
	}
	void start() {
    CkCallback startCb(CkIndex_CCVertex::start(), g);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
	}

	void getScannedVertexNum() {
		g.getScannedVertexNum();
	}
	void verify() {
		g.verify();
	}
};

struct CCEdge {
	CmiUInt8 v;

	CCEdge() {}
	CCEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class CCVertex : public CBase_CCVertex {
	private:
		std::vector<CCEdge> adjlist;
		CmiUInt8 componentId;

	public:
		CCVertex() : componentId(thisIndex) {
			// Contribute to a reduction to signal the end of the setup phase
			//contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
		}

		void connectVertex(const CCEdge & edge) {
			if (edge.v > CkNumPes() * N) 
				CkAbort("Incorrect dest vertex ID");
			adjlist.push_back(edge);
		}

		void process(const CCEdge & edge) {
			connectVertex(edge);
		}

		CCVertex(CkMigrateMessage *msg) {}

		void start() {
			ArrayMeshStreamer<dtype, long long, CCVertex, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();

			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				localAggregator->insertData(dtype(this->componentId, RADIX), it->v);
			}
		}

		void update(const CmiUInt8 & c, const int & r) {
			if (c < componentId) {
				//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
				this->componentId = c;
				if (r > 0) {
					ArrayMeshStreamer<dtype, long long, CCVertex, SimpleMeshRouter>
						* localAggregator = aggregator.ckLocalBranch();
					typedef typename std::vector<CCEdge>::iterator Iterator; 
					for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						localAggregator->insertData(dtype(this->componentId, r - 1), it->v);
					}
				} else {
					typedef typename std::vector<CCEdge>::iterator Iterator; 
					for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[it->v].update(this->componentId, RADIX);
					}
					//driverProxy.grayVertexExist();
				}
			}
		}

		void process(const dtype &data) {
			update(data.c, data.r);
		}

		void getScannedVertexNum() {
			CmiUInt8 c = 1/*(parent == -1 ? 0 : 1)*/;
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
		}

		void verify() {
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) 
				thisProxy[it->v].check(componentId);
		}

		void check(const CmiUInt8 & c) {
			CkAssert(componentId == c);
		}
};

typedef GraphLib::GraphGenerator<
	CCGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "tram_cc.def.h"
