#include <GraphGenerator.h>
#include <common.h>
#include <sstream>
#include "charm_cc.decl.h"

CProxy_TestDriver driverProxy;

struct CCEdge {
	CmiUInt8 v;
	double w;

	CCEdge() {}
	CCEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w;
	}
};

class CCVertex : public CBase_CCVertex {
private:
	std::vector<CCEdge> adjlist;
	CmiUInt8 componentId;

public:
  CCVertex() {
		componentId = thisIndex;
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const CCEdge & edge) {
		adjlist.push_back(edge);
	}

  CCVertex(CkMigrateMessage *msg) {}

	void start() {
		// broadcast
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(componentId);
		}
	}

  void update(const CmiUInt8 & c) {

		if (c < componentId) {
			// update current componentId
			componentId = c;
			// broadcast
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v].update(componentId);
			}
		}
  }

	void verify() {
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].check(componentId);
		}
		//contribute(CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void check(CmiUInt8 c) {
		CkAssert(componentId == c);
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %lld, %s\n", thisIndex, componentId, ss.str().c_str());
	}
};


class TestDriver : public CBase_TestDriver {
private:
  CProxy_CCVertex  g;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_CCVertex, CCEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;

    // Create the chares storing vertices
    g  = CProxy_CCVertex::ckNew(opts.N);
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_CCVertex, CCEdge, Options>::ckNew(g, opts); 

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("CC running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator.generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initialization completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		g.start();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		if (opts.verify) g.verify();
		CkStartQD(CkIndex_TestDriver::done(), &thishandle);
  }

	void done() {
		double update_walltime = CkWallTimer() - starttime;
		//double gteps = 1e-9 * globalNubScannedVertices * 1.0/update_walltime;
		CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
		//CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNubScannedVertices, (double)globalNubScannedVertices*100/opts.N);
		//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
		//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
		//		gteps / CkNumPes());
		//g.print();
		CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
	}

	void exit() {
		CkExit();
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

#include "charm_cc.def.h"
