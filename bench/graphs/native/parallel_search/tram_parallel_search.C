#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class	BFSGraph;

typedef struct __dtype {
	CmiUInt8 parent; 
	int r;
	__dtype() {}
  __dtype(CmiUInt8 parent, int r) : 
		parent(parent), r(r) {}
	void pup(PUP::er &p) {
		p | parent;
		p | r;
	}
} dtype;

#include "tram_parallel_search.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, BFSVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

class BFSGraph : public GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::Charm> {
public:
	BFSGraph() : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	BFSGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(nVertex) {

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, g, 1);
	}
	BFSGraph(const CProxy_BFSVertex & g) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void start(CmiUInt8 root) {
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::exit(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::exit(), driverProxy);
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

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSVertex : public CBase_BFSVertex {
	private:
		std::vector<BFSEdge> adjlist;
		enum State {White, Gray, Black} state;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : state(White), parent(-1), numScannedEdges(0) {
			// Contribute to a reduction to signal the end of the setup phase
			//contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
		}

		void connectVertex(const BFSEdge & edge) {
			if (edge.v > CkNumPes() * N) 
				CkAbort("Incorrect dest vertex ID");
			adjlist.push_back(edge);
		}

		void process(const BFSEdge & edge) {
			connectVertex(edge);
		}

		BFSVertex(CkMigrateMessage *msg) {}

		void make_root() {
			CkAssert(state != Gray);

			if (state)
				return;
			state = Black;
			parent = thisIndex;

			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				localAggregator->insertData(dtype(thisIndex, RADIX), it->v);
				//thisProxy[it->v].process(dtype(thisIndex, RADIX));
				numScannedEdges++;
			}
		}

		void process(const dtype &data) {
			if (state)
				return;
			//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
			//state = Gray;
			parent = data.parent;
			if (data.r > 0) {
				state = Black;

				ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
					* localAggregator = aggregator.ckLocalBranch();
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					localAggregator->insertData(dtype(thisIndex, data.r - 1), it->v);
					//thisProxy[it->v].process(dtype(thisIndex, data.r - 1));
				}
			} else {
				//state = Gray;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) 
					thisProxy[it->v].process(dtype(thisIndex, RADIX));
			}
		}

		void getScannedVertexNum() {
			CmiUInt8 c = (state == Black ? 1 : 0);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
		}

		void verify() {
			CkAssert(state != Gray);
			if (state == Black)
				thisProxy[parent].check();
		}

		void check() {
			CkAssert(state == Black);
		}

		//void foo() {}
};

typedef GraphLib::GraphGenerator<
BFSGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"
#include "tram_parallel_search.def.h"
