//#define CMK_TRAM_VERBOSE_OUTPUT
#include "NDMeshStreamer.h"
#include "GraphLib.h"
#include "common.h"

typedef CmiUInt8 dtype;

class BFSVertex;
class BFSEdge;
class	CProxy_BFSVertex;
typedef GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> BFSGraph;

#include "tram_parallel_search.decl.h"

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
	bool visited;
	CmiUInt8 parent;
	CmiUInt8 numScannedEdges;

public:
  BFSVertex() : visited(false), parent(-1), numScannedEdges(0) {

    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void process(const BFSEdge & edge) {
		connectVertex(edge);
	}

	void connectVertex(const BFSEdge & edge) {
		adjlist.push_back(edge);
	}

  BFSVertex(CkMigrateMessage *msg) {}

  inline void process(const dtype  &parent) {
		if (visited)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		visited = true;
		this->parent = parent;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
    ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			//thisProxy[*it].update(); 
      localAggregator->insertData(thisIndex, it->v);
			numScannedEdges++;
		}
		//localAggregator->done();
  }

  void make_root() {
		if (visited)
			return;
		//CkPrintf("%d: updated\n", thisIndex);
		visited = true;
    ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
      * localAggregator = aggregator.ckLocalBranch();

		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
      localAggregator->insertData(thisIndex, it->v);
			numScannedEdges++;
		}
  }

	void getScannedEdgesNum() {
    contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
               CkCallback(CkReductionTarget(TestDriver, done),
                          driverProxy));
	}

	void getScannedVertexNum() {
		CmiUInt8 c = (visited ? 1 : 0);
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done),
					driverProxy));
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

		// Different mechanisms of graph generation
		
		// 1. uchares randomly generate adjlists and connect themselves to peers 
		//g->createGraph();

		// 2. use graphio lib to load graph (not implemented)
		//io.loadGraphToChares();

		// 3. generate stream of edges in mainchare and send them to uchares
		//CProxy_GraphGen generator = CProxy_GraphGen::ckNew(g, scale);

		generator->generate();

		//CkCallback cb(CkIndex_TestDriver::start(), thisProxy);
		//uchareset_proxy.run(0, cb);
		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
		CkPrintf("root=%lld\n", root);
    starttime = CkWallTimer();

		//g[root].make_root();
    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		BFSGraph::Proxy & g = graph->getProxy();
		//g.getScannedEdgesNum();
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 total) {
		CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total, 
				N, 100.0*total/N, M, 100.0*total/M, root);
		if (total < 0.25 * N) {
			starttime = CkWallTimer();
			CkPrintf("restart test, root=%lld\n", root);
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			//double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			//CkPrintf("Scanned edges = %lld\n", globalNumScannedEdges);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());
			CkExit();
		}
  }
};

#include "tram_parallel_search.def.h"
