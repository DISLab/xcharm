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

#include "charm_bfs_sync_radix.decl.h"

CmiUInt8 N, M;
int K = 16;
//int R = 128;
int R = 64;
CProxy_TestDriver driverProxy;

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
		enum State {White, Gray, Black} state;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : level(-1), state(White), parent(-1), numScannedEdges(0) {
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
			//CkPrintf("%d: updated\n", thisIndex);
			this->level = 0;
			this->state = Black;
			typedef typename std::vector<BFSEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				if (it->v != thisIndex)
					thisProxy[it->v].update(this->level, thisIndex, R);
			}
		}

		void update(int level, CmiUInt8 parent, int r) {
			if ((this->level >= 0) && (this->level <= level + 1))
				return;
			state = Gray;
			this->level = level + 1;
			this->parent = parent;

			if (r > 0) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[it->v].update(this->level, thisIndex, r-1);
						//CkPrintf("%d: update %d\n", thisIndex, *it);
						numScannedEdges++;
						//getProxy().flush();
				}
			} else
				state = Gray;
		}

		void resume() {
			if (state == Gray) {
				state = Black;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[it->v].update(this->level, thisIndex, R);
						//CkPrintf("%d: update %d\n", thisIndex, *it);
						numScannedEdges++;
						//getProxy().flush();
				}
				driverProxy.grayVertexExist();
			}
		}

		void getScannedEdgesNum() {
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, done),
						driverProxy));
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
		GraphLib::TransportType::/*Tram*/Charm> Generator;
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
		CkPrintf("[done]\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		CkPrintf("Start breadth-first search:......\n");
    starttime = CkWallTimer();
		g[root].make_root();

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
			CkExit();
		}
  }

	void exit() {
		CkAbort("exit: must be never called\n");
	}

};

#include "charm_bfs_sync_radix.def.h"
