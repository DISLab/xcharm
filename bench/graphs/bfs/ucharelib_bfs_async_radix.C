#include <uChareLib.h>
#include <GraphGenerator.h>
#include <common.h>

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
	int size() const { 
		return sizeof(v);
	}
	static void pack(const BFSEdge * edge, void * buf) {
		*(CmiUInt8 *)buf = edge->v;
	}
	static void unpack(BFSEdge * edge, const void * buf) {
		edge->v = *(CmiUInt8 *)buf;
	}
};

#include "ucharelib_bfs_async_radix.decl.h"

CmiUInt8 N, M;
int K = 16;
int R = 128;

CProxy_TestDriver driverProxy;

class BFSVertex : public CBase_uChare_BFSVertex {
	private:
		std::vector<CmiUInt8> adjlist;
		enum State {White, Gray, Black} state;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex(const uChareAttr_BFSVertex & attr) : state(White), parent(-1), numScannedEdges(0),
			CBase_uChare_BFSVertex(attr) {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			// Contribute to a reduction to signal the end of the setup phase
			contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
		}

		void connectVertex(const BFSEdge & edge) {
			if (edge.v > CkNumPes() * N) 
				CkAbort("Incorrect dest vertex ID");
			adjlist.push_back(edge.v);
			//CkPrintf("[vertex:%lld] connected to %lld\n", thisIndex, vertexId);
		}

		void update(int r) {
			if (state)
				return;

			//CkPrintf("%d (pe=%d): updated, radius %d\n", thisIndex, getuChareSet()->getPe(), r);
			state = Gray;

			if (r > 0) {
				state = Black;
				typedef typename std::vector<CmiUInt8>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[*it]->update(r-1);
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
				typedef typename std::vector<CmiUInt8>::iterator Iterator; 
				for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
						thisProxy[*it]->update(R);
						//CkPrintf("%d: update %d\n", thisIndex, *it);
						numScannedEdges++;
						//getProxy().flush();
				}

				driverProxy.grayVertexExist();
			}
		}

		void getScannedEdgesNum() {
			//contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
			//           CkCallback(CkReductionTarget(TestDriver, done),
			//                      driverProxy));
			contribute(numScannedEdges, CkReduction::sum_long, 
										CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
		}
};


class TestDriver : public CBase_TestDriver {
private:
	CProxy_uChare_BFSVertex g;
	double startt;
	double endt;

	CmiUInt8 root;
  double starttime;
	bool complete;

	Options opts;

	CProxy_GraphGenerator<CProxy_uChare_BFSVertex, BFSEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) : complete(false) {
		parseCommandOptions(args->argc, args->argv, opts);
		N = opts.N;
		M = opts.M;
		root = opts.root;
    driverProxy = thishandle;

	  g = CProxy_uChare_BFSVertex::ckNew(N, CkNumPes()); 

		// create graph generator
		generator = CProxy_GraphGenerator<CProxy_uChare_BFSVertex, BFSEdge, Options>::ckNew(g, opts); 

    delete args;
  }

	void init() {
		CkPrintf("TestDriver::init called\n");
		//TODO: is it possible to move to the Main::Main?
		g.run(CkCallback(CkIndex_TestDriver::startGraphConstruction(), thisProxy));
	}

  void startGraphConstruction() {
		CkPrintf("BFS running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator.generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}

  void start() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("[done]\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		CkPrintf("Start breadth-first search:......\n");
    starttime = CkWallTimer();
		g[root]->update(R);

		CkStartQD(CkIndex_TestDriver::resume(), &thishandle);
  }

	void resume() {
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


	void checkErrors() {
		//bfsvertex_array.checkErrors();
		//CkStartQD(CkIndex_TestDriver::reportErrors(), &thishandle);
	}

  void reportErrors(CmiInt8 globalNumErrors) {
    //CkPrintf("Found %lld errors in %lld locations (%s).\n", globalNumErrors,
    //         tableSize, globalNumErrors <= 0.01 * tableSize ?
    //         "passed" : "failed");
    CkExit();
  }

	void exit() {
		CkAbort("exit: must be never called\n");
	}

};

#include "ucharelib_bfs_async_radix.def.h"
