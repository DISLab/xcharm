#include <GraphLib.h>
#include <common.h>

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
	CmiUInt8 parent; 
	int r;
	__dtype() {}
  __dtype(CmiUInt8 parent, int r) : 
		parent(parent), r(r) {}
	void pup(PUP::er &p) {
		p | parent;
		p | r;
	}
} dtype;

#include "tram_bfs_async_radix.decl.h"

CmiUInt8 N, M;
int K = 16;
//int R = 128;
int R = 2;
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
		enum State {White, Gray, Black} state;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : state(White), parent(-1), numScannedEdges(0) {
			// Contribute to a reduction to signal the end of the setup phase
			//contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
		}

		void connectVertex(const BFSEdge & edge) {
			if (edge.v > CkNumPes() * N) 
				CkAbort("Incorrect dest vertex ID");
			adjlist.push_back(edge);
		}

		void process(const BFSEdge & edge) {
			connectVertex(edge);
		}

		BFSVertex(CkMigrateMessage *msg) {}

		void make_root() {
			CkAssert(state != Gray);

			if (state)
				return;
			state = Black;
			parent = thisIndex;

			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				localAggregator->insertData(dtype(thisIndex, R), it->v);
				//thisProxy[it->v].process(dtype(thisIndex, R));
				numScannedEdges++;
			}
		}

		void process(const dtype &data) {
			if (state)
				return;
			//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
			//state = Gray;
			parent = data.parent;
			if (data.r > 0) {
				state = Black;

				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
					* localAggregator = aggregator.ckLocalBranch();
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					localAggregator->insertData(dtype(thisIndex, data.r - 1), it->v);
					//thisProxy[it->v].process(dtype(thisIndex, data.r - 1));
					numScannedEdges++;
				}
			} else {
				state = Gray;
				driverProxy.grayVertexExist();
			}
		}

		void resume() {
			if (state == Gray) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
					* localAggregator = aggregator.ckLocalBranch();
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					localAggregator->insertData(dtype(thisIndex, R), it->v);
					//thisProxy[it->v].process(dtype(thisIndex, R));
					numScannedEdges++;
				}
				//driverProxy.grayVertexExist();
			}
		}

		void getScannedEdgesNum() {
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
		}

		void getScannedVertexNum() {
			CmiUInt8 c = (state == Black ? 1 : 0);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
		}

		void verify() {
			CkAssert(state != Gray);
			if (state == Black)
				thisProxy[parent].check();
		}

		void check() {
			CkAssert(state == Black);
		}

		//void foo() {}
};


class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime, startt;
	double endt;
	bool complete;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
		Options, 
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::Tram> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) : complete(false) {
		parseCommandOptions(args->argc, args->argv, opts);
		N = opts.N;
		M = opts.M;
		root = opts.root;
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

    starttime = CkWallTimer();
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
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		CkPrintf("Start breadth-first search:......\n");
    starttime = CkWallTimer();

    CkCallback startCb(CkIndex_BFSVertex::make_root(), g[root]);
    //CkCallback startCb(CkIndex_BFSVertex::foo(), g[root]);
		//g[root].make_root();
    CkCallback endCb(CkIndex_TestDriver::exit(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::resume(), &thishandle);
  }

	void resume() {
		BFSGraph::Proxy & g = graph->getProxy();
		if (!complete) {
			complete = true;
			g.resume();
			CkStartQD(CkIndex_TestDriver::resume(), &thishandle);
		}
		else startVerificationPhase();
	}

	void grayVertexExist() {
		complete = false;
	}

  void startVerificationPhase() {
		BFSGraph::Proxy & g = graph->getProxy();
		//g.getScannedEdgesNum();
		g.verify();
		CkStartQD(CkCallbackResumeThread());
		g.getScannedVertexNum();
  }

  void done(CmiUInt8 total) {

		CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total, 
				N, 100.0*total/N, M, 100.0*total/M, root);
		if (total < 0.25 * N) {
			//root = rand_64(gen) % N;
			root = rand() % N;

			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			//double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());
			CkExit();
		}
  }

	void exit() {
		CkAbort("exit: must be never called\n");
	}
};

#include "tram_bfs_async_radix.def.h"
