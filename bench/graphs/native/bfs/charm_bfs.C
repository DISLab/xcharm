#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class BFSGraph;

#include "charm_bfs.decl.h"

class BFSGraph : public GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::Charm> {
public:
	bool directed;
public:
	BFSGraph(bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	BFSGraph(CmiUInt8 nVertex, bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	BFSGraph(const CProxy_BFSVertex & g, bool directed = true) : 
		directed(directed),
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
	void dump_edges() {
		g.dump_edges();
	}
};

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

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
	int level;
	CmiUInt8 parent;

public:
  BFSVertex() : level(-1), parent(-1) {
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
		//CkPrintf("%d: updated\n", thisIndex);
		this->level = 0;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			if (it->v != thisIndex)
				thisProxy[it->v].update(this->level, thisIndex);
		}
	}

  void update(int level, CmiUInt8 parent) {
		if ((this->level >= 0) && (this->level <= level + 1))
			return;
		//CkPrintf("%d: updated level(%d->%d), parent(%lld->%lld)\n", 
		//		thisIndex, this->level, level, this->parent, parent);
		this->level = level + 1;
		this->parent = parent;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(this->level, thisIndex);
		}
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
	GraphLib::GraphGeneratorType::GRAPH,
	GraphLib::TransportType::Tram
	> Generator;
#include "driver.C"
#include "charm_bfs.def.h"
