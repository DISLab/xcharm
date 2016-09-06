#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class BFSGraph;

#include "charm_parallel_search.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

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
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	BFSGraph(const CProxy_BFSVertex & g) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void start(CmiUInt8 root) {
		g[root].make_root();
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		g[root].make_root();
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
	bool visited;
	CmiUInt8 parent;
	CmiUInt8 numScannedEdges;

public:
  BFSVertex() : visited(false), parent(-1), numScannedEdges(0) {
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const BFSEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const BFSEdge & edge) {
		connectVertex(edge);
	}

  BFSVertex(CkMigrateMessage *msg) {}

	void make_root() {
		if (visited)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		visited = true;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update();
		}
	}

  void update() {
		if (visited)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		visited = true;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update();
		}
  }

	void getScannedEdgesNum() {
    contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
               CkCallback(CkReductionTarget(TestDriver, done),
                          driverProxy));
	}

	void getScannedVertexNum() {
		CmiUInt8 c = (visited ? 1 : 0);
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done),
					driverProxy));
	}

	void print() {
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		CkPrintf("vid:%lld (", thisIndex);
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) { 
			CkPrintf("%lld,", it->v);
		}
		CkPrintf(")\n");
	}

	void verify() {}
	void check() {}
};

typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "charm_parallel_search.def.h"
