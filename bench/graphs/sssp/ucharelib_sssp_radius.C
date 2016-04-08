#include <uChareLib.h>
#include <GraphGenerator.h>
#include <common.h>

struct SSSPEdge {
	CmiUInt8 v;
	double w;

	SSSPEdge() {}
	SSSPEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w; 
	}
	int size() const { 
		return sizeof(v) + sizeof(w);
	}
	static void pack(const SSSPEdge * edge, void * buf) {
		*(CmiUInt8 *)buf = edge->v;
		buf = (char *)buf + sizeof(CmiUInt8);
		*(double *)buf = edge->w;
	}
	static void unpack(SSSPEdge * edge, const void * buf) {
		edge->v = *(CmiUInt8 *)buf;
		buf = (char *)buf + sizeof(CmiUInt8);
		edge->w = *(double *)buf;
	}
};

#include "ucharelib_sssp_radius.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

class SSSPVertex : public CBase_uChare_SSSPVertex {
private:
	std::vector<SSSPEdge> adjlist;
	double weight;
	CmiUInt8 parent;

public:
  SSSPVertex(const uChareAttr_SSSPVertex & attr) : weight(std::numeric_limits<double>::max()), parent(-1),
 			CBase_uChare_SSSPVertex(attr)	{

    // Contribute to a reduction to signal the end of the setup phase
		contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
  }

	void connectVertex(const SSSPEdge & edge) {
		adjlist.push_back(edge);
	}

	void make_root() {
		weight = 0;
		parent = thisIndex;
		// broadcast
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v]->update(thisIndex, weight + it->w);
		}
	}

  void update(const CmiUInt8 & v, const double & w) {

		if (w < weight) {
			
			CkPrintf("%lld: %2.2f --> %2.2f, parent %lld\n", thisIndex, weight, w, v);

			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v]->update(thisIndex, weight + it->w);
			}
		}
  }

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent]->check(weight);
		
		CmiUInt8 c = (parent == -1 ? 0 : 1);	
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, 
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void check(const double & w) {
		CkAssert(weight < w);
	}
};

class TestDriver : public CBase_TestDriver {
private:
  CProxy_uChare_SSSPVertex  g;
	CmiUInt8 root;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_uChare_SSSPVertex, SSSPEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
		N = opts.N;
		M = opts.M;
		root = opts.root;
    driverProxy = thishandle;

    // Create the chares storing vertices
    g  = CProxy_uChare_SSSPVertex::ckNew(N, CkNumPes());
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_uChare_SSSPVertex, SSSPEdge, Options>::ckNew(g, opts); 

    delete args;
  }

	void init() {
		CkPrintf("TestDriver::init called\n");
		//TODO: is it possible to move to the Main::Main?
		g.run(CkCallback(CkIndex_TestDriver::startGraphConstruction(), thisProxy));
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
		g[root]->make_root();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		g.verify();
		//CkStartQD(CkIndex_TestDriver::done(), &thishandle);
  }

  void done(CmiUInt8 globalNubScannedVertices) {
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNubScannedVertices, (double)globalNubScannedVertices*100/N);
		if (globalNubScannedVertices < 0.25 * N) {
			//root = rand_64(gen) % N;
			root = rand() % N;
			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNubScannedVertices * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNubScannedVertices, (double)globalNubScannedVertices*100/N);
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

#include "ucharelib_sssp_radius.def.h"
