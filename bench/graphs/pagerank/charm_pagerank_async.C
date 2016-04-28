#include <GraphGenerator.h>
#include <common.h>
#include <sstream>
#include <map>
#include "charm_pagerank_async.decl.h"

CmiUInt8 N;
double D;
CProxy_TestDriver driverProxy;

struct PageRankEdge {
	CmiUInt8 v;
	double w;

	PageRankEdge() {}
	PageRankEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w;
	}
};

class PageRankVertex : public CBase_PageRankVertex {
private:
	std::vector<PageRankEdge> adjlist;
	// key -- PageRank iteration number
	// value -- pair of
	//		first -- number of received messages for this iteration
	//		second -- accumulated sum
	std::map<int, std::pair<int, double> > sumInPageRankValues;
	double rank;
	int icount;

public:
  PageRankVertex() : icount(0) {
		rank = 1.0 / N;
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const PageRankEdge & edge) {
		adjlist.push_back(edge);
	}

  PageRankVertex(CkMigrateMessage *msg) {}

	void start() {
		// broadcast
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(icount, rank / adjlist.size());
		}
	}

  void update(const int & iter, const double & r) {

		sumInPageRankValues[iter].first++; 
		sumInPageRankValues[iter].second += D * r; 

		//CkPrintf("%d: update called (%d, %e), %d, %d\n", thisIndex, iter, r,
		//		sumInPageRankValues[icount].first, adjlist.size());

		if (sumInPageRankValues[icount].first == adjlist.size()) {
			rank = (1.0 - D)/N + sumInPageRankValues[icount].second;
			if (++icount < 10) {
				typedef typename std::vector<PageRankEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					thisProxy[it->v].update(icount, rank / adjlist.size());
				}
			}
		} 
  }

	void verify() {
		CkAssert((0 < rank) && (rank < 1));
		//contribute(CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %2.2f, %s\n", thisIndex, rank, ss.str().c_str());
	}
};


class TestDriver : public CBase_TestDriver {
private:
  CProxy_PageRankVertex  g;
  double starttime;
	Options opts;

	CProxy_GraphGenerator<CProxy_PageRankVertex, PageRankEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;
		N = opts.N;
		D = 0.85; 

    // Create the chares storing vertices
    g  = CProxy_PageRankVertex::ckNew(opts.N);
		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_PageRankVertex, PageRankEdge, Options>::ckNew(g, opts); 

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
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
		CkPrintf("Run PageRank\n");
    starttime = CkWallTimer();
		//for (int i = 0; i < 10; i++) {
			//CkPrintf("PageRank step %d:\n", i);
			// do pagerank step 
			g.start();
			// wait for current step to be done 
			//CkStartQD(CkCallbackResumeThread());
		//}
		startVerificationPhase();
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
};

#include "charm_pagerank_async.def.h"
