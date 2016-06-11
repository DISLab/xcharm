#include <uChareLib.h>
#include <GraphGenerator.h>
#include <common.h>

struct PageRankEdge {
	CmiUInt8 v;
	double w;

	PageRankEdge() {}
	PageRankEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w; 
	}
	int size() const { 
		return sizeof(v) + sizeof(w);
	}
	static void pack(const PageRankEdge * edge, void * buf) {
		*(CmiUInt8 *)buf = edge->v;
		buf = (char *)buf + sizeof(CmiUInt8);
		*(double *)buf = edge->w;
	}
	static void unpack(PageRankEdge * edge, const void * buf) {
		edge->v = *(CmiUInt8 *)buf;
		buf = (char *)buf + sizeof(CmiUInt8);
		edge->w = *(double *)buf;
	}
};

#include "ucharelib_pagerank.decl.h"

CmiUInt8 N;
double D;
CProxy_TestDriver driverProxy;

class PageRankVertex : public CBase_uChare_PageRankVertex {
private:
	std::vector<PageRankEdge> adjlist;
	double rankOld, rankNew;

public:
  PageRankVertex(const uChareAttr_PageRankVertex & attr) : CBase_uChare_PageRankVertex(attr)	{
		rankNew = 1.0 / N;
    // Contribute to a reduction to signal the end of the setup phase
		contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
  }

	void connectVertex(const PageRankEdge & edge) {
		adjlist.push_back(edge);
	}

	void doPageRankStep_init() {
		// set initial page rank values
		rankOld = rankNew;
		rankNew = (1.0 - D) / N;
	}

	void doPageRankStep_update() {
		// broadcast
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v]->update(rankOld / adjlist.size());
		}
	}

  void update(const double & r) {
		rankNew += D * r;
  }

	void verify() {
		CkAssert((0 < rankNew) && (rankNew < 1));
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%lld: %2.2f, %s\n", thisIndex, rankNew, ss.str().c_str());
	}
};

class TestDriver : public CBase_TestDriver {
private:
  CProxy_uChare_PageRankVertex  g;
	CmiUInt8 root;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_uChare_PageRankVertex, PageRankEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;
		N = opts.N;
		D = 0.85; 

    // Create the chares storing vertices
    g  = CProxy_uChare_PageRankVertex::ckNew(opts.N, CkNumPes());
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_uChare_PageRankVertex, PageRankEdge, Options>::ckNew(g, opts); 

    delete args;
  }

	void init() {
		CkPrintf("TestDriver::init called\n");
		//TODO: is it possible to move to the Main::Main?
		g.run(CkCallback(CkIndex_TestDriver::startGraphConstruction(), thisProxy));
	}

  void startGraphConstruction() {
		CkPrintf("PageRank running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator.generate();

		CkStartQD(CkIndex_TestDriver::doPageRank(), &thishandle);
	}

  void doPageRank() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initialization completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		for (int i = 0; i < 3; i++) {
			CkPrintf("PageRank step %d:\n", i);
			// do pagerank step initilization
			g.doPageRankStep_init();
			// wait for current step to be done 
			CkStartQD(CkCallbackResumeThread());
			// do pagerank step 
			g.doPageRankStep_update();
			// wait for current step to be done 
			CkStartQD(CkCallbackResumeThread());
		}
		startVerificationPhase();
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
		//g.print();
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

#include "ucharelib_pagerank.def.h"
