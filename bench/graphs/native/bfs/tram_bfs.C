//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
#include "GraphLib.h"
#include "common.h"

class BFSVertex;
class BFSEdge;
class	CProxy_BFSVertex;
typedef GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> BFSGraph;

typedef struct __dtype {
	int level; 
	CmiUInt8 parent; 
	__dtype() {}
  __dtype(int level, CmiUInt8 parent) : 
		level(level), parent(parent) {}
	void pup(PUP::er & p) {
		p | level;
		p | parent;
	}
} dtype;
#include "tram_bfs.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, BFSVertex,
                         SimpleMeshRouter> aggregator;
// Max number of keys buffered by communication library
const int numMsgsBuffered = 1024;

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSVertex : public CBase_BFSVertex {
private:
	std::vector<BFSEdge> adjlist;
	int level;
	CmiUInt8 parent;

public:
  BFSVertex() : level(-1), parent(-1) {

    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const BFSEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const BFSEdge & edge) {
		connectVertex(edge);
	}

  BFSVertex(CkMigrateMessage *msg) {}

  inline void process(const dtype  &data) {
		const int & level = data.level;
		const CmiUInt8 & parent = data.parent;
		if ((this->level >= 0) && (this->level <= level + 1))
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		this->level = level + 1;
		this->parent = parent;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
    ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			localAggregator->insertData(dtype(this->level, thisIndex), it->v);
		}
		//localAggregator->done();
  }

  void make_root() {
		if (level >= 0)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		level = 0;
    ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
      localAggregator->insertData(dtype(this->level, thisIndex), it->v);
		}
  }

	void getScannedVertexNum() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent].check(level);
	}
	void check(int level) {
		CkAssert(this->level + 1 == level);
	}
};

class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
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
    driverProxy = thishandle;

    // Create graph
    graph = new BFSGraph(CProxy_BFSVertex::ckNew(opts.N));
		// create graph generator
		generator = new Generator(*graph, opts);

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      ::ckNew(numMsgsBuffered, 2, dims, graph->getProxy(), 1);

		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("BFS running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator->generate();
		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}

  void start() {
		srandom(1);
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
    CkPrintf("start, root = %lld\n", root);
    starttime = CkWallTimer();

		//g[root].make_root();
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		BFSGraph::Proxy & g = graph->getProxy();
		root = random() % N;
    CkPrintf("restart, root = %lld\n", root);
    starttime = CkWallTimer();

		//g[root].make_root();
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		BFSGraph::Proxy & g = graph->getProxy();
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 globalNumScannedVertex) {
		BFSGraph::Proxy & g = graph->getProxy();
		if (globalNumScannedVertex < 0.25 * N) {
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedVertex * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned vertices = %lld\n", globalNumScannedVertex);
			CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
							 gteps / CkNumPes());
			if (opts.verify) { 
				CkPrintf("Start verification...\n");
				g.verify();
			}
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() {
		CkPrintf("Done.\n");
		CkExit();
	}
};

#include "tram_bfs.def.h"
