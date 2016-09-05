#include <GraphLib.h>
#include <common.h>
#include <sstream>

class SSSPVertex;
class SSSPEdge;
class	CProxy_SSSPVertex;
typedef GraphLib::Graph<
	SSSPVertex,
	SSSPEdge,
	CProxy_SSSPVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> SSSPGraph;
#include "charm_sssp.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

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
	CmiUInt8 totalUpdates;

public:
  SSSPVertex() : weight(std::numeric_limits<double>::max()), parent(-1), totalUpdates(0) {
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const SSSPEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const SSSPEdge & edge) {
		connectVertex(edge);
	}

  SSSPVertex(CkMigrateMessage *msg) {}

	void make_root() {
		weight = 0;
		parent = thisIndex;
		// broadcast
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(thisIndex, weight + it->w);
		}
	}

  void update(const CmiUInt8 & v, const double & w) {
		if (w < weight) {
			//CkPrintf("%d: %2.2f -> %2.2f\n", thisIndex, weight, w);
			totalUpdates++;

			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v].update(thisIndex, weight + it->w);
			}
		}
  }

	void getScannedVertexNum() {
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
	CmiUInt8 root;
  double starttime;
	Options opts;

	SSSPGraph *graph;
	typedef GraphLib::GraphGenerator<
		SSSPGraph, 
		Options, 
		GraphLib::VertexMapping::SingleVertex,
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::Tram> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		CkPrintf("TestDriver running...\n");
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
    driverProxy = thishandle;

    // Create the chares storing vertices
    // Create graph
    graph = new SSSPGraph(CProxy_SSSPVertex::ckNew(opts.N));
		// create graph generator
		generator = new Generator(*graph, opts);

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("SSSP running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator->generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}

  void start() {
		srandom(1);
		SSSPGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initialization completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
    CkPrintf("start, root = %lld\n", root);
    starttime = CkWallTimer();
		g[root].make_root();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		SSSPGraph::Proxy & g = graph->getProxy();
		root = random() % N;
    CkPrintf("restart, root = %lld\n", root);
    starttime = CkWallTimer();
		g[root].make_root();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		SSSPGraph::Proxy & g = graph->getProxy();
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 globalNumScannedVertices) {
		SSSPGraph::Proxy & g = graph->getProxy();
		CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNumScannedVertices, (double)globalNumScannedVertices*100/opts.N);
		if (globalNumScannedVertices < 0.25 * opts.N) {
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedVertices * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNumScannedVertices, (double)globalNumScannedVertices*100/opts.N);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());

			//g.print();
			//g.countTotalUpdates(CkCallback(CkReductionTarget(TestDriver, printTotalUpdates), driverProxy));

			if (opts.verify) {
				CkPrintf("Start verification...\n");
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
};

#include "charm_sssp.def.h"
