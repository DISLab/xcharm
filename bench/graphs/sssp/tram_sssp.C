//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
#include <GraphGenerator.h>
#include <common.h>
#include <sstream>

#define RADIUS
#define R 4

typedef struct __dtype {
	CmiUInt8 v;
	double w;
#if defined (RADIUS)
	int r;
	__dtype (CmiUInt8 v, double w, int r) : v(v), w(w), r(r) {}
#else
	__dtype (CmiUInt8 v, double w) : v(v), w(w) {}
#endif
	__dtype() {}
	void pup(PUP::er & p) { 
		p | v;
		p | w;
#if defined (RADIUS)
		p | r;
#endif
	}
} dtype;
#include "tram_sssp.decl.h"

CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, int, SSSPVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

struct SSSPEdge {
	CmiUInt8 v;
	double w;

	SSSPEdge() {}
	SSSPEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w;
	}
};

class SSSPVertex : public CBase_SSSPVertex {
private:
	std::vector<SSSPEdge> adjlist;
	double weight;
	CmiUInt8 parent;

public:
  SSSPVertex() : weight(std::numeric_limits<double>::max()), parent(-1) {

    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const SSSPEdge & edge) {
		adjlist.push_back(edge);
	}

  SSSPVertex(CkMigrateMessage *msg) {}

	void make_root() {
		weight = 0;
		parent = thisIndex;
    ArrayMeshStreamer<dtype, int, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();
		// broadcast
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
			localAggregator->insertData(dtype(thisIndex, weight + it->w, R), it->v);
			//thisProxy[it->v].update(dtype(thisIndex, weight + it->w, R));
#else
			localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
			//thisProxy[it->v].update(dtype(thisIndex, weight + it->w));
#endif
		}
	}

	inline void process(const dtype & m) {
		const CmiUInt8 & v = m.v;
		const double & w = m.w;
#if defined (RADIUS)
		const int & r = m.r;
#endif

		//CkAbort("process called");

    ArrayMeshStreamer<dtype, int, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		if (w < weight) {

			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
				if (r > 0 )
					localAggregator->insertData(dtype(thisIndex, weight + it->w, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(thisIndex, weight + it->w, R));
#else
				localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
#endif
			}
		}
  }

	void update(const dtype & m) {
		const CmiUInt8 & v = m.v;
		const double & w = m.w;
#if defined (RADIUS)
		const int & r = m.r;
#endif
    ArrayMeshStreamer<dtype, int, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		//CkPrintf("%d: update called\n", thisIndex);

		if (w < weight) {
			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
#if defined (RADIUS)
				if (r > 0 )
					localAggregator->insertData(dtype(thisIndex, weight + it->w, r - 1), it->v);
				else
					thisProxy[it->v].update(dtype(thisIndex, weight + it->w, R));
#else
				localAggregator->insertData(dtype(thisIndex, weight + it->w), it->v);
				//thisProxy[it->v].update(dtype(thisIndex, weight + it->w));
#endif
			}
		}
  }

	void countScannedVertices() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);	
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, 
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent].check(weight);
	}

	void check(double w) {
		CkAssert(weight < w);
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %.2f, %lld, %s\n", thisIndex, weight, parent, ss.str().c_str());
	}
};


class TestDriver : public CBase_TestDriver {
private:
  CProxy_SSSPVertex  g;
	CmiUInt8 root;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_SSSPVertex, SSSPEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
		root = opts.root;
    driverProxy = thishandle;

    // Create the chares storing vertices
    g  = CProxy_SSSPVertex::ckNew(opts.N);
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_SSSPVertex, SSSPEdge, Options>::ckNew(g, opts); 

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, int, SSSPVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, g, 1);

		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("SSSP running...\n");
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

		//g[root].make_root();
    CkCallback startCb(CkIndex_SSSPVertex::make_root(), g[root]);
    //CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    CkCallback endCb(CkIndex_TestDriver::foo(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::countScannedVertices(), &thishandle);
  }

  void countScannedVertices() {
		g.countScannedVertices();
  }

  void done(CmiUInt8 nScanned) {
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", nScanned, (double)nScanned*100/opts.N);
		if (nScanned < 0.25 * opts.N) {
			//root = rand_64(gen) % N;
			root = rand() % opts.N;
			starttime = CkWallTimer();
			CkPrintf("Restarting test...\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * nScanned * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", nScanned, (double)nScanned*100/opts.N);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());

			//print();

			if (opts.verify) {
				CkPrintf("Run verification...\n");
				g.verify();
			}
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() {
		CkPrintf("Done. Exit.\n");
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

#include "tram_sssp.def.h"
