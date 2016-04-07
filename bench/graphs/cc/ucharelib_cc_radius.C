#include <uChareLib.h>
#include <GraphGenerator.h>
#include <common.h>

struct CCEdge {
	CmiUInt8 v;
	double w;

	CCEdge() {}
	CCEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w; 
	}
	int size() const { 
		return sizeof(v) + sizeof(w);
	}
	static void pack(const CCEdge * edge, void * buf) {
		*(CmiUInt8 *)buf = edge->v;
		buf = (char *)buf + sizeof(CmiUInt8);
		*(double *)buf = edge->w;
	}
	static void unpack(CCEdge * edge, const void * buf) {
		edge->v = *(CmiUInt8 *)buf;
		buf = (char *)buf + sizeof(CmiUInt8);
		edge->w = *(double *)buf;
	}
};

#include "ucharelib_cc.decl.h"

CProxy_TestDriver driverProxy;

class CCVertex : public CBase_uChare_CCVertex {
private:
	std::vector<CCEdge> adjlist;
	CmiUInt8 componentId;

public:
  CCVertex(const uChareAttr_CCVertex & attr) : CBase_uChare_CCVertex(attr)	{
		componentId = thisIndex;
    // Contribute to a reduction to signal the end of the setup phase
		contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
  }

	void connectVertex(const CCEdge & edge) {
		adjlist.push_back(edge);
	}

	void start() {
		// broadcast
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v]->update(componentId);
		}
	}

  void update(const CmiUInt8 & c) {
		if (c < componentId) {
			// update current componentId
			componentId = c;
			// broadcast
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v]->update(componentId);
			}
		}
  }

	void verify() {
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v]->check(componentId);
		}
	}

	void check(const CmiUInt8 & c) {
		CkAssert(componentId == c);
	}
};

class TestDriver : public CBase_TestDriver {
private:
  CProxy_uChare_CCVertex  g;
	CmiUInt8 root;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_uChare_CCVertex, CCEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;

    // Create the chares storing vertices
    g  = CProxy_uChare_CCVertex::ckNew(opts.N, CkNumPes());
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_uChare_CCVertex, CCEdge, Options>::ckNew(g, opts); 

    delete args;
  }

	void init() {
		//TODO: is it possible to move to the Main::Main?
		g.run(CkCallback(CkIndex_TestDriver::startGraphConstruction(), thisProxy));
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
    CkPrintf("root = %lld\n", root);
    starttime = CkWallTimer();
		g.start();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		g.verify();
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

#include "ucharelib_cc.def.h"
