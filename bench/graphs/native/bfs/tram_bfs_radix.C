#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class	BFSGraph;

#define RADIX_ENABLED

typedef struct __dtype {
	int level;
	CmiUInt8 parent; 
	int r;
  __dtype(int level, CmiUInt8 parent, int r) : 
		level(level), parent(parent), r(r) {}
	__dtype() {}
	void pup(PUP::er & p) {
		p | level;
		p | parent;
		p | r;
	}
} dtype;

#include "tram_bfs_radix.decl.h"

CmiUInt8 N, M;
int K = 16;
int R = 1;
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
		// Instantiate communication library group with a handle to the client
		aggregator =
			CProxy_ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
			::ckNew(numMsgsBuffered, 2, dims, g, 1);
	}
	void start(CmiUInt8 root) {
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
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
	void dump_edges() {
		g.dump_edges();
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
		CmiUInt8 parent;
		int level;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : level(-1), parent(-1), numScannedEdges(0) {
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
			CkPrintf("R = %d\n", R);
			if (level >= 0)
				return;
			level = 0;
			parent = thisIndex;
			ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();

			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				localAggregator->insertData(dtype(this->level, thisIndex, R), it->v);
				numScannedEdges++;
			}
		}

		void update(int level, CmiUInt8 parent, int r) {

			if ((this->level >= 0) && (this->level <= level + 1))
				return;
			//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
			this->level = level + 1;
			this->parent = parent;

			if (r > 0) {
				ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
					* localAggregator = aggregator.ckLocalBranch();
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					if (thisProxy.ckLocalBranch()->lastKnown(it->v) != CmiMyPe())
						//localAggregator->insertData(dtype(this->level, thisIndex, r - 1), it->v);
						localAggregator->insertData(dtype(this->level, thisIndex, 1), it->v);
					else
						thisProxy[it->v].update(this->level, thisIndex, R);
				}
			} else {
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					thisProxy[it->v].update(this->level, thisIndex, R);
				}
				//driverProxy.grayVertexExist();
			}
		}

		void process(const dtype &data) {
			update(data.level, data.parent, data.r);
		}

		void getScannedVertexNum() {
			CmiUInt8 c = (parent == -1 ? 0 : 1);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
		}

		void verify() {
			if ((parent != -1) && (parent != thisIndex))
				thisProxy[parent].check(level);
		}

		void check(int level) {
			CkAssert(this->level + 1 == level);
		}
		void dump_edges() {
			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				CkPrintf("%lld,%lld\n", thisIndex, it->v);
			}
		}
};

typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::GRAPH,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "tram_bfs_radix.def.h"
