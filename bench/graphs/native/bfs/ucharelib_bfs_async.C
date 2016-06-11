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

#include "ucharelib_bfs_async.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

class BFSVertex : public CBase_uChare_BFSVertex {
	private:
		std::vector<CmiUInt8> adjlist;
		bool visited;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex(const uChareAttr_BFSVertex & attr) : visited(false), parent(-1), numScannedEdges(0),
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
			//CkPrintf("[vertex:% d] connected to %d\n", thisIndex, vertexId);
		}

		void verify() {
			typedef typename std::vector<CmiUInt8>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) 
				if (*it > CkNumPes() * N) 
					CkAbort("---->Incorrect dest vertex ID");
			CkPrintf("[vertex:% d] verified\n", thisIndex);
		}

		void update() {
			if (visited)
				return;
			//CkPrintf("%d (pe=%d): updated\n", thisIndex, getuChareSet()->getPe());
			visited = true;
			typedef typename std::vector<CmiUInt8>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
					thisProxy[*it]->update();
					numScannedEdges++;
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
	Options opts;

	CProxy_GraphGenerator<CProxy_uChare_BFSVertex, BFSEdge, Options> generator;

public:
  TestDriver(CkArgMsg* args) {
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
		;
		
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();

		generator.generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}

	void exit() {
		CkAbort("exit: must be never called\n");
	}

  void start() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("[done]\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		CkPrintf("Start breadth-first search:......");
    starttime = CkWallTimer();
		g[root]->update();
		//g->verify();

		CkStartQD(CkIndex_TestDriver::testCompletion(), &thishandle);
  }

	void testCompletion() {
		CkPrintf("[done]\n");
		if (opts.verify)
			g.getScannedEdgesNum();
		//uchareset_proxy.gatherPendingMessagesNum(CkCallback(CkReductionTarget(TestDriver, testCompletion2), thisProxy));
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
};

#include "ucharelib_bfs_async.def.h"
