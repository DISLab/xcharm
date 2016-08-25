#include <sstream>
#include <GraphLib.h>
#include <common.h>

#ifndef GRAPH_TRANSPORT
	#define GRAPH_TRANSPORT Tram
#endif
	
#ifndef GENERATOR_TRANSPORT
	#define GENERATOR_TRANSPORT Tram
#endif

class PageRankVertex;
class PageRankEdge;
class	CProxy_PageRankVertex;

typedef GraphLib::Graph<
	PageRankVertex,
	PageRankEdge,
	CProxy_PageRankVertex,
	GraphLib::TransportType::GRAPH_TRANSPORT
	> PageRankGraph;

#include "pagerank.decl.h"

CmiUInt8 N;
double D;
CProxy_TestDriver driverProxy;

/**
 * User edge class 
 * TODO: add GraphLib::Edge as a base class for user edge
 **/
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

/**
 * User vertex class 
 **/
class PageRankVertex : public GraphLib::Vertex<PageRankVertex, 
	PageRankEdge, GraphLib::TransportType::GRAPH_TRANSPORT> {
private:
	double rankOld, rankNew;

public:
  PageRankVertex() {
		rankNew = 1.0 / N;
  }
  PageRankVertex(CkMigrateMessage *msg) {}

	//TODO: get rid of this method
	void process(const PageRankEdge & edge) {
		addEdge(edge);
	}
	
	void doPageRankStep_init() {
		// set initial page rank values
		rankOld = rankNew;
		rankNew = (1.0 - D) / N;
	}

	void doPageRankStep_update() {
		CmiUInt8 v;
		FORALL_ADJ_VERTICES(v, PageRankVertex) {
			sendMessage(rankOld / getEdgeNumber(), v);
		}
	}

  void process(const double & r) {
		rankNew += D * r;
  }

	void verify() {
		CkAssert((0 < rankNew) && (rankNew < 1));
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
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
		GraphLib::TransportType::GENERATOR_TRANSPORT
		> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    driverProxy = thishandle;
		N = opts.N;
		D = 0.85; 

    // Create graph
    graph = new PageRankGraph(opts.N);
		// Create graph generator
		generator = new Generator(*graph, opts);
		// Wait till all initialization related messages will be delivered
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("PageRank running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		// Start graph generation
		generator->generate();
		// Wait till generation is done
		CkStartQD(CkIndex_TestDriver::doPageRank(), &thishandle);
	}

  void doPageRank() {
		PageRankGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initialization completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    starttime = CkWallTimer();
		for (int i = 0; i < 10; i++) {
			CkPrintf("PageRank step %d:\n", i);
			// do pagerank step initialization
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
		if (opts.verify) {
			g.verify();
			CkPrintf("Verify\n");
		}
		CkStartQD(CkIndex_TestDriver::done(), &thishandle);
  }

	void done() {
		double update_walltime = CkWallTimer() - starttime;
		CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
		CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
	}

	void doneVerified() {
		double update_walltime = CkWallTimer() - starttime;
		CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
		CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
	}

	void exit() {
		CkExit();
	}
};

#include "pagerank.def.h"
