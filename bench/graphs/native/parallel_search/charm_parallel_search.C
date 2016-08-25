#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class	CProxy_BFSVertex;
typedef GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> BFSGraph;

#include "charm_parallel_search.decl.h"

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

  void update() {
		if (visited)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		visited = true;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update();
			numScannedEdges++;
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
};


class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
		Options, 
		GraphLib::VertexMapping::SingleVertex,
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
    graph = new BFSGraph(CProxy_BFSVertex::ckNew(opts.N));
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
		srandom(1);
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
		CkPrintf("start, root=%lld\n", root);
    starttime = CkWallTimer();
		g[root].update();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

	void restart() {
		BFSGraph::Proxy & g = graph->getProxy();
		root = random() % N;
		CkPrintf("restart, root=%lld\n", root);
    starttime = CkWallTimer();
		g[root].update();
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
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			//double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			//CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedEdges, (double)globalNumScannedEdges*100/M);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());
			//CkExit();
			//g.print();
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() { CkExit(); }
};

//#include "charm_parallel_search.def.h"
