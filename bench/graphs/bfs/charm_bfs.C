#include <GraphGenerator.h>
#include <common.h>
#include "charm_bfs.decl.h"

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
  CProxy_BFSVertex  g;
	CmiUInt8 root;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_BFSVertex, BFSEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
		root = opts.root;
    driverProxy = thishandle;

    // Create the chares storing vertices
    g  = CProxy_BFSVertex::ckNew(N);
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_BFSVertex, BFSEdge, Options>::ckNew(g, opts); 

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

		generator.generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		g[root].update();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		g.getScannedEdgesNum();
  }

  void done(CmiUInt8 globalNumScannedEdges) {
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


	void checkErrors() {
		//g.checkErrors();
		//CkStartQD(CkIndex_TestDriver::reportErrors(), &thishandle);
	}

  void reportErrors(CmiInt8 globalNumErrors) {
    //CkPrintf("Found %lld errors in %lld locations (%s).\n", globalNumErrors,
    //         tableSize, globalNumErrors <= 0.01 * tableSize ?
    //         "passed" : "failed");
    CkExit();
  }
};

#include "charm_bfs.def.h"
