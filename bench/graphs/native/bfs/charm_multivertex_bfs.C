#include <GraphLib.h>
#include <common.h>
#include <deque>

typedef struct __dtype {
	CmiUInt8 v;
	int level;
	CmiUInt8 parent;
	__dtype() {}
	__dtype(CmiUInt8 v, int level, CmiUInt8 parent) : v(v), level(level), parent(parent) {}
	void pup(PUP::er & p) { p | v; p | level; p | parent; }
} dtype;

class BFSMultiVertex;
class BFSEdge;
class	CProxy_BFSMultiVertex;
typedef GraphLib::Graph<
	BFSMultiVertex,									// Vertex
	//std::pair<CmiUInt8, BFSEdge>,   // Edge
	BFSEdge,   // Edge
	CProxy_BFSMultiVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> BFSGraph;

#include "charm_multivertex_bfs.decl.h"

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

class BFSVertex {
	private:
		CmiUInt8 thisIndex;
		std::vector<BFSEdge> adjlist;
		int level;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : thisIndex(-1), level(-1), 
			parent(-1), numScannedEdges(0) {}
		BFSVertex(CmiUInt8 idx) : thisIndex(idx), level(-1), 
			parent(-1), numScannedEdges(0) {}
		void connectVertex(const BFSEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(BFSMultiVertex & multiVertex, int level, CmiUInt8 parent); 
		void verify(BFSMultiVertex & multiVertex); 
		void check(int level); 
		const CmiUInt8 getScannedEdgesNum() { return numScannedEdges; }
		const CmiUInt8 getScannedVertexNum() { return (level >= 0 ? 1 : 0); }
};

class BFSMultiVertex : public CBase_BFSMultiVertex {
	private:
		std::vector<BFSVertex> vertices;
		std::deque<dtype> q;

	public:
		BFSMultiVertex() {
			//vertices.assign(N / ckGetArraySize(), BFSVertex());
			for(CmiUInt8 i = 0; i < N / ckGetArraySize(); i++)
				vertices.push_back(BFSVertex(getBase() + i));
		}
		/*BFSMultiVertex(CmiUInt8 n) {
			vertices.assign(n, BFSVertex());
		}*/

		void connectVertex(const std::pair<CmiUInt8, BFSEdge> & edge) {
			vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
		}

		void process(const std::pair<CmiUInt8, BFSEdge > & edge) {
			connectVertex(edge);
		}

		BFSMultiVertex(CkMigrateMessage *msg) {}

		inline std::deque<dtype> & getQ() { return q; }

		void make_root(const CmiUInt8 & root) {
			CkAssert(getBaseIndex(root) == thisIndex);
			update(root, 0, root);
		}

		void update(const CmiUInt8 & v, int level, CmiUInt8 parent) {

			q.push_back(dtype(v, level, parent));
			while (!q.empty()) {
				dtype data = q.front();
				CkAssert(getBaseIndex(data.v) == thisIndex);

				q.pop_front();
				vertices[getLocalIndex(data.v)].update(*this, data.level, data.parent);
			}
		}

		void verify() {
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				dtype data = q.front();
				CkAssert(getBaseIndex(data.v) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(data.v)].check(data.level);
			}
		}

		void check(const CmiUInt8 & v, int level) {
			vertices[getLocalIndex(v)].check(level);
		}

		void getScannedEdgesNum() {
			CmiUInt8 numScannedEdges = 0;
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				numScannedEdges += it->getScannedEdgesNum();
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
								 CkCallback(CkReductionTarget(TestDriver, done),
														driverProxy));
		}

		void getScannedVertexNum() {
			CmiUInt8 numScannedVertices = 0;
			typedef std::vector<BFSVertex>::iterator Iterator;
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
};

void BFSVertex::update(BFSMultiVertex & multiVertex, int level, CmiUInt8 parent) {
	if ((this->level >= 0) && (this->level <= level + 1))
		return;

	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	this->level = level + 1;
	this->parent = parent;

	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(dtype(it->v, this->level, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v, this->level, thisIndex);
		numScannedEdges++;
	}
}

void BFSVertex::verify(BFSMultiVertex & multiVertex) {
	if (level < 0)
		return;

	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 

	if ((parent != -1) && (parent != thisIndex))
		if (multiVertex.isLocalIndex(parent))
			q.push_back(dtype(parent, level, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(parent)].check(parent, level);
}

void BFSVertex::check(int level) {
	CkAssert(this->level == level - 1);
}


class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
		Options, 
		GraphLib::VertexMapping::MultiVertex,
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::Tram> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
    driverProxy = thishandle;

    // Create graph
    graph = new BFSGraph(CProxy_BFSMultiVertex::ckNew(CmiNumPes()));
		// create graph generator
		generator = new Generator(*graph, opts);

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("BFS running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator->generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
    CkPrintf("root = %lld\n", root);
    starttime = CkWallTimer();
		g[root / (N / CmiNumPes())].make_root(root);
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		BFSGraph::Proxy & g = graph->getProxy();
		//g.getScannedEdgesNum();
		g.getScannedVertexNum();
  }

	void done(CmiUInt8 total) {
		BFSGraph::Proxy & g = graph->getProxy();
		CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total, 
				N, 100.0*total/N, M, 100.0*total/M, root);

		if (total < 0.25 * N) {
			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			//double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			//CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedEdges, (double)globalNumScannedEdges*100/M);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());
			if (opts.verify) { 
				CkPrintf("Start verification...\n");
				g.verify();
			}
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() {
		CkPrintf("Done.\n");
		CkExit();
	}
};

#include "charm_multivertex_bfs.def.h"