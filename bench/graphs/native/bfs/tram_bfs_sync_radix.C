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
	int level;
	CmiUInt8 parent; 
	int r;
  __dtype(int level, CmiUInt8 parent, int r) : 
		level(level), parent(parent), r(r) {}
	__dtype() {}
	void pup(PUP::er & p) {
		p | level;
		p | parent;
		p | r;
	}
} dtype;

#include "tram_bfs_sync_radix.decl.h"

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
		int level;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : state(White), level(-1), parent(-1), numScannedEdges(0) {
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
			if (level >= 0)
				return;
			level = 0;
			state = Black;
			parent = thisIndex;
			ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();

			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				localAggregator->insertData(dtype(this->level, thisIndex, R), it->v);
				numScannedEdges++;
			}
		}

		void process(const dtype &data) {
			const int & level = data.level;
			const CmiUInt8 & parent = data.parent;
			const int & r = data.r;

			if ((this->level >= 0) && (this->level <= level + 1))
				return;
			//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
			this->level = level + 1;
			this->parent = parent;

			if (r > 0) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				ArrayMeshStreamer<dtype, long long, BFSVertex, SimpleMeshRouter>
					* localAggregator = aggregator.ckLocalBranch();
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					localAggregator->insertData(dtype(this->level, thisIndex, r - 1), it->v);
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
					//thisProxy[it->v].update(R);
					localAggregator->insertData(dtype(this->level, thisIndex, R), it->v);
					numScannedEdges++;
				}
			}
		}

		void getScannedEdgesNum() {
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
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
		g.getScannedEdgesNum();
  }

  void done(CmiUInt8 globalNumScannedEdges) {
		BFSGraph::Proxy & g = graph->getProxy();
		if (globalNumScannedEdges < 0.25 * M) {
			//root = rand_64(gen) % N;
			root = rand() % N;
			starttime = CkWallTimer();
			CkPrintf("restart test\n");
			driverProxy.start();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned edges = %lld\n", globalNumScannedEdges);
			CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
							 gteps / CkNumPes());
			if (opts.verify) { 
				CkPrintf("Start verification...\n");
				g.verify();
			}
			CkExit();
		}
  }

	void exit() {
		CkAbort("exit: must be never called\n");
	}
};

#include "tram_bfs_sync_radix.def.h"
