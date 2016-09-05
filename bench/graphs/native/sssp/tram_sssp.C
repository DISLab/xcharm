//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
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

#define RADIUS
#define R 1

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

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, SSSPVertex,
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
    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
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

		//CkPrintf("%d: process called\n", thisIndex);

    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		if (w < weight) {
			totalUpdates++;

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

	void update(const dtype & m) {
		const CmiUInt8 & v = m.v;
		const double & w = m.w;
#if defined (RADIUS)
		const int & r = m.r;
#endif
    ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		//CkPrintf("%d: update called\n", thisIndex);

		if (w < weight) {
			totalUpdates++;

		//CkPrintf("%d: %2.2f -> %2.2f\n", thisIndex, weight, w);
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

	void getScannedVertexNum() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);	
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, 
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void countTotalUpdates(const CkCallback & cb) {
		contribute(sizeof(totalUpdates), &totalUpdates, CkReduction::sum_long, cb);
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
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
		root = opts.root;
    driverProxy = thishandle;

    // Create graph
    graph = new SSSPGraph(CProxy_SSSPVertex::ckNew(opts.N));
		// create graph generator
		generator = new Generator(*graph, opts);

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, long long, SSSPVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, graph->getProxy(), 1);

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

		//g[root].make_root();
    CkCallback startCb(CkIndex_SSSPVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    //CkCallback endCb(CkIndex_TestDriver::foo(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		SSSPGraph::Proxy & g = graph->getProxy();
		root = random() % N;
    CkPrintf("restart, root = %lld\n", root);
    starttime = CkWallTimer();

		//g[root].make_root();
    CkCallback startCb(CkIndex_SSSPVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		SSSPGraph::Proxy & g = graph->getProxy();
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 globalNumScannedVertex) {
		SSSPGraph::Proxy & g = graph->getProxy();
		CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNumScannedVertex, (double)globalNumScannedVertex*100/opts.N);
		if (globalNumScannedVertex < 0.25 * opts.N) {
			//root = rand_64(gen) % N;
			root = rand() % opts.N;
			starttime = CkWallTimer();
			CkPrintf("Restarting test...\n");
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedVertex * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned vertices = %lld (%.0f%%)\n", globalNumScannedVertex, (double)globalNumScannedVertex*100/opts.N);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());

			//print();

			//g.countTotalUpdates(CkCallback(CkReductionTarget(TestDriver, printTotalUpdates), driverProxy));

			if (opts.verify) {
				CkPrintf("start verification...\n");
				g.verify();
			}
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() {
		CkPrintf("Done. Exit.\n");
		CkExit();
	}

	void printTotalUpdates(CmiUInt8 nUpdates) {
		CkPrintf("nUpdates = %lld\n", nUpdates);
	}

	void foo() {CkAbort("foo called");}
};

#include "tram_sssp.def.h"
