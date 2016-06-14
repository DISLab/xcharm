#include <GraphLib.h>
#include <common.h>
#include <sstream>

class PageRankVertex;
class PageRankEdge;
class	CProxy_PageRankVertex;
typedef GraphLib::Graph<
	PageRankVertex,
	PageRankEdge,
	CProxy_PageRankVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> PageRankGraph;

#include "charm_pagerank_intracount.decl.h"

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
	double rank0, rank1;
	int icount, mcount;

public:
  PageRankVertex() : icount(0), mcount(0) {

		double & rankOld = (icount % 2) ? rank0 : rank1;
		double & rankNew = (icount % 2) ? rank1 : rank0;

		rankOld = 1.0 / N;
		rankNew = (1.0 - D) / N;

    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const PageRankEdge & edge) {
		adjlist.push_back(edge);
		mcount++;
	}

	void process(const PageRankEdge & edge) {
		connectVertex(edge);
	}

  PageRankVertex(CkMigrateMessage *msg) {}

	void doPageRankStep() {
		double & rankOld = (icount % 2) ? rank0 : rank1;
		// broadcast
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(rankOld / adjlist.size());
		}
	}

  void update(const double & r) {

		double & rankNew = (icount % 2) ? rank1 : rank0;
		rankNew += D * r;
		if (!--mcount) {
			mcount = adjlist.size();
			icount++;
			double & rankOld = (icount % 2) ? rank0 : rank1;
			double & rankNew = (icount % 2) ? rank1 : rank0;
			rankNew = (1.0 - D) / N;
		}
  }

	void verify() {
		double & rankOld = (icount % 2) ? rank0 : rank1;
		CkAssert((0 < rankOld) && (rankOld < 1));
		//contribute(CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		double & rankOld = (icount % 2) ? rank0 : rank1;
		double & rankNew = (icount % 2) ? rank1 : rank0;
		CkPrintf("%d: %2.2f, %s\n", thisIndex, rankOld, ss.str().c_str());
	}
};


class TestDriver : public CBase_TestDriver {
private:
  double starttime;
	Options opts;

	PageRankGraph *graph;
	typedef GraphLib::GraphGenerator<
		PageRankGraph, 
		Options, 
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::/*Tram*/Charm> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;
		N = opts.N;
		D = 0.85; 

    // Create graph
    graph = new PageRankGraph(CProxy_PageRankVertex::ckNew(opts.N));
		// create graph generator
		generator = new Generator(*graph, opts);

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

		generator->generate();

		CkStartQD(CkIndex_TestDriver::doPageRank(), &thishandle);
	}


  void doPageRank() {
		PageRankGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initialization completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		for (int i = 0; i < 3; i++) {
			CkPrintf("PageRank step %d:\n", i);
			// do pagerank step 
			g.doPageRankStep();
			// wait for current step to be done 
			CkStartQD(CkCallbackResumeThread());
		}
		startVerificationPhase();
  }

  void startVerificationPhase() {
		PageRankGraph::Proxy & g = graph->getProxy();
		if (opts.verify) g.verify();
		CkStartQD(CkIndex_TestDriver::done(), &thishandle);
  }

	void done() {
		PageRankGraph::Proxy & g = graph->getProxy();
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

#include "charm_pagerank_intracount.def.h"
