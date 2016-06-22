//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
//#include <GraphGenerator.h>
#include <GraphLib.h>
#include <common.h>
#include <sstream>

typedef struct __dtype {
	double rank;
	__dtype () {}
	__dtype (double rank) : rank(rank) {}
	void pup(PUP::er & p) { 
		p | rank;
	}
} dtype;

class PageRankVertex;
class PageRankEdge;
class	CProxy_PageRankVertex;
typedef GraphLib::Graph<
	PageRankVertex,
	PageRankEdge,
	CProxy_PageRankVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> PageRankGraph;

#include "tram_pagerank.decl.h"

CmiUInt8 N;
double D;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, int, PageRankVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

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
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
		rankNew = 1.0 / N;
  }

	void connectVertex(const PageRankEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const PageRankEdge & edge) {
		connectVertex(edge);
	}

  PageRankVertex(CkMigrateMessage *msg) {}

	void doPageRankStep_init() {
		// set initial page rank values
		rankOld = rankNew;
		rankNew = (1.0 - D) / N;
	}

	void doPageRankStep_update() {
    ArrayMeshStreamer<dtype, int, PageRankVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();
		// broadcast
		typedef typename std::vector<PageRankEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			//thisProxy[it->v].update(rankOld / adjlist.size());
			localAggregator->insertData(dtype(rankOld / adjlist.size()), it->v);
		}
	}

	inline void process(const dtype & m) {
		rankNew += D * m.rank;
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
    //g  = CProxy_PageRankVertex::ckNew(opts.N);

		// create graph generator
		generator = new Generator(*graph, opts);

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, int, PageRankVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, graph->getProxy(), 1);

		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("PageRank/TRAM running...\n");
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
		for (int i = 0; i < 10; i++) {
			CkPrintf("PageRank step %d:\n", i);
			// do pagerank step initilization
			g.doPageRankStep_init();
			// wait for current step to be done 
			CkStartQD(CkCallbackResumeThread());
			// do pagerank step 

			//g.doPageRankStep_update();

			CkCallback startCb(CkIndex_PageRankVertex::doPageRankStep_update(), g);
			//CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
			CkCallback endCb(CkIndex_TestDriver::foo(), driverProxy);
			aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

			// wait for current step to be done 
			CkStartQD(CkCallbackResumeThread());
		}
		startVerificationPhase();
  }

  void startVerificationPhase() {
		PageRankGraph::Proxy & g = graph->getProxy();
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

	void foo() {CkAbort("foo called");}
};

#include "tram_pagerank.def.h"
