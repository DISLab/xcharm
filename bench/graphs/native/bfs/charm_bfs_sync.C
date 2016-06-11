#include <GraphGenerator.h>
#include <common.h>
#include "charm_bfs_sync.decl.h"

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
    CkPrintf("root = %lld\n", root);
    starttime = CkWallTimer();
		g[root].make_root();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 globalNumScannedVertices) {
		CkPrintf("globalNumScannedVertices = %lld\n", globalNumScannedVertices);
		if (globalNumScannedVertices < 0.25 * N) {
			//root = rand_64(gen) % N;
			root = rand() % N;
			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedVertices * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedVertices, (double)globalNumScannedVertices*100/M);
			CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
							 gteps / CkNumPes());
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

#include "charm_bfs_sync.def.h"
