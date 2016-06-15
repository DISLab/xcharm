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

#include "charm_bfs_async.decl.h"

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
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::/*Tram*/Charm> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
		root = opts.root;
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
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		g[root].update();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		BFSGraph::Proxy & g = graph->getProxy();
		g.getScannedEdgesNum();
  }

  void done(CmiUInt8 globalNumScannedEdges) {
		BFSGraph::Proxy & g = graph->getProxy();
		if (globalNumScannedEdges < 0.25 * M) {
			//root = rand_64(gen) % N;
			root = rand() % N;
			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedEdges, (double)globalNumScannedEdges*100/M);
			CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
							 gteps / CkNumPes());
			CkExit();
		}
  }
};

#include "charm_bfs_async.def.h"
