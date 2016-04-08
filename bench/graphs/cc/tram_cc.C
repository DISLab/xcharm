//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
#include <GraphGenerator.h>
#include <common.h>
#include <sstream>

#define RADIUS
#define R 100

typedef struct __dtype {
	CmiUInt8 c;
#if defined (RADIUS)
	int r;
	__dtype (CmiUInt8 c, int r) : c(c), r(r) {}
#else
	__dtype (CmiUInt8 c) : c(c) {}
#endif
	__dtype() {}
	void pup(PUP::er & p) { 
		p | c;
#if defined (RADIUS)
		p | r;
#endif
	}
} dtype;
#include "tram_cc.decl.h"

CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, int, CCVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

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
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
		componentId = thisIndex;
  }

	void connectVertex(const CCEdge & edge) {
		adjlist.push_back(edge);
	}

  CCVertex(CkMigrateMessage *msg) {}

	void start() {
    ArrayMeshStreamer<dtype, int, CCVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();
		// broadcast
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
			localAggregator->insertData(dtype(componentId, R), it->v);
#else
			localAggregator->insertData(dtype(componentId), it->v);
#endif
		}
	}

	inline void process(const dtype & m) {
		const CmiUInt8 & c = m.c;
#if defined (RADIUS)
		const int & r = m.r;
#endif

    ArrayMeshStreamer<dtype, int, CCVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		if (c < componentId) {
			// update current weight and parent
			componentId = c;
			// broadcast
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
				if (r > 0 )
					localAggregator->insertData(dtype(componentId, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(componentId, R));
#else
				localAggregator->insertData(dtype(componentId), it->v);
#endif
			}
		}
  }

	void update(const dtype & m) {
		const CmiUInt8 & c = m.c;
#if defined (RADIUS)
		const int & r = m.r;
#endif
    ArrayMeshStreamer<dtype, int, CCVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		if (c < componentId) {
			// update current weight and parent
			componentId = c;
			// broadcast
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
				if (r > 0 )
					localAggregator->insertData(dtype(componentId, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(componentId, R));
#else
				localAggregator->insertData(dtype(componentId), it->v);
#endif
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
		if (componentId != c)
			CkPrintf("%d: componentId incorrect componentId = %lld, c= %lld\n", thisIndex, componentId, c);
		//CkAssert(componentId == c);
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

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, int, CCVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, g, 1);

		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("CC/TRAM running...\n");
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

    CkCallback startCb(CkIndex_CCVertex::start(), g);
    //CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    CkCallback endCb(CkIndex_TestDriver::foo(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		//aggregator.finish();
		//aggregator.syncInit();
		//aggregator.flushToIntermediateDestinations();
		if (opts.verify) g.verify();
		CkStartQD(CkIndex_TestDriver::done(), &thishandle);
  }

	void done() {
		double update_walltime = CkWallTimer() - starttime;
		//double gteps = 1e-9 * globalNubScannedVertices * 1.0/update_walltime;
		CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
		//CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNubScannedVertices, (double)globalNubScannedVertices*100/N);
		//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
		//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
		//				 gteps / CkNumPes());

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

	void foo() {CkAbort("foo called");}
};

#include "tram_cc.def.h"
