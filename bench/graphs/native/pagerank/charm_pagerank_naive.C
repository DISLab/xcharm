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

#include "charm_pagerank_naive.decl.h"

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
	double rankOld, rankNew;

public:
  PageRankVertex() {
		rankNew = 1.0 / N;
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }
  PageRankVertex(CkMigrateMessage *msg) {}

	void connectVertex(const PageRankEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const PageRankEdge & edge) {
		connectVertex(edge);
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
			thisProxy[it->v].update(rankOld / adjlist.size());
		}
	}

  void update(const double & r) {
		rankNew += D * r;
  }

	void verify() {
		CkAssert((0 < rankNew) && (rankNew < 1));
		//contribute(CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %2.2f, %s\n", thisIndex, rankNew, ss.str().c_str());
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
		GraphLib::VertexMapping::SingleVertex,
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::Tram> Generator;
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
		while (opts.freeze_after_graph_creation) {}
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
		while(1){}
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
		PageRankGraph::Proxy & g = graph->getProxy();
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

#include "charm_pagerank_naive.def.h"
