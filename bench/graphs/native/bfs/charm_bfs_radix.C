#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class BFSGraph;

#include "charm_bfs_radix.decl.h"

CmiUInt8 N, M;
int K = 16;
//int R = 128;
int R = 64;
CProxy_TestDriver driverProxy;
CProxy_Master masterProxy;

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
	BFSGraph(const CProxy_BFSVertex & g) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(g)	{
		masterProxy = CProxy_Master::ckNew(g); 
	}
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
		int level;
		enum State {White, Gray, Black} state;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : level(-1), state(White), parent(-1), numScannedEdges(0) {
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
			//CkPrintf("%d: updated\n", thisIndex);
			this->level = 0;
			this->state = Black;
			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				if (it->v != thisIndex)
					thisProxy[it->v].update(this->level, thisIndex, R);
			}
		}

		void update(int level, CmiUInt8 parent, int r) {
			if ((this->level >= 0) && (this->level <= level + 1))
				return;
			state = Gray;
			this->level = level + 1;
			this->parent = parent;

			if (r > 0) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[it->v].update(this->level, thisIndex, r-1);
						numScannedEdges++;
				}
			} else
				state = Gray;
		}

		void resume() {
			if (state == Gray) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[it->v].update(this->level, thisIndex, R);
						numScannedEdges++;
				}
				masterProxy.notify();
			}
		}

		void getScannedVertexNum() {
			CmiUInt8 c = (parent == -1 ? 0 : 1);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
		}

		void getScannedEdgesNum() {
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
		}

		void verify() {
			if ((parent != -1) && (parent != thisIndex))
				thisProxy[parent].check(level);
		}

		void check(int level) {
			CkAssert(this->level + 1 == level);
		}
};

class Master : public CBase_Master {
private:
	bool complete;
	CProxy_BFSVertex g;

public:
	Master(CProxy_BFSVertex & g) : g(g), complete(false) {}
	void resume() {
		if (!complete) {
			complete = true;
			g.resume();
			CkStartQD(CkIndex_Master::resume(), &thishandle);
		}
		else CkAbort("execution complete");
	}
	void notify() {
		complete = false;
	}
};

#include "driver.C"
#include "charm_bfs_radix.def.h"
