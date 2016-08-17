#include <GraphLib.h>
#include <common.h>
#include <deque>

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

#include "charm_multivertex_parallel_search.decl.h"

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
		std::vector<BFSEdge> adjlist;
		bool visited;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : visited(false), parent(-1), numScannedEdges(0) {}
		void connectVertex(const BFSEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(BFSMultiVertex & multiVertex); 
		void verify(BFSMultiVertex & multiVertex); 
		void check(); 
		const CmiUInt8 getScannedEdgesNum() { return numScannedEdges; }
		const CmiUInt8 getScannedVertexNum() { return (visited ? 1 : 0); }
};

class BFSMultiVertex : public CBase_BFSMultiVertex {
	private:
		std::vector<BFSVertex> vertices;
		std::deque<CmiUInt8> q;

	public:
		BFSMultiVertex() {
			vertices.assign(N / ckGetArraySize(), BFSVertex());
		}
		BFSMultiVertex(CmiUInt8 n) {
			vertices.assign(n, BFSVertex());
		}

		void connectVertex(const std::pair<CmiUInt8, BFSEdge> & edge) {
			vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
		}

		void process(const std::pair<CmiUInt8, BFSEdge > & edge) {
			connectVertex(edge);
		}

		BFSMultiVertex(CkMigrateMessage *msg) {}

		inline std::deque<CmiUInt8> & getQ() { return q; }

		void update(const CmiUInt8 & v) {

			q.push_back(v);
			while (!q.empty()) {
				CmiUInt8 u = q.front();
				CkAssert(getBaseIndex(u) == thisIndex);

				q.pop_front();
				vertices[getLocalIndex(u)].update(*this);
			}
		}

		void verify() {
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				CmiUInt8 u = q.front();
				CkAssert(getBaseIndex(u) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(u)].check();
			}
		}

		void check(const CmiUInt8 & v) {
			vertices[getLocalIndex(v)].check();
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
};

void BFSVertex::update(BFSMultiVertex & multiVertex) {
	if (visited)
		return;

	std::deque<CmiUInt8> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	visited = true;

	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(it->v);
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v);
		numScannedEdges++;
	}
}

void BFSVertex::verify(BFSMultiVertex & multiVertex) {
	if (!visited)
		return;
	std::deque<CmiUInt8> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(it->v);
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].check(it->v);
	}
}

void BFSVertex::check() {
	CkAssert(visited);
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
		CkPrintf("root=%lld\n", root);
    starttime = CkWallTimer();
		g[root / (N / CmiNumPes())].update(root);
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

#include "charm_multivertex_parallel_search.def.h"
