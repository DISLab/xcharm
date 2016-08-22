#include <GraphLib.h>
#include <common.h>
#include <deque>


typedef struct __dtype {
	CmiUInt8 v;
	CmiUInt8 parent;
	__dtype() {}
	__dtype(CmiUInt8 v, CmiUInt8 parent) : v(v), parent(parent) {}
	void pup(PUP::er & p) { p | v; p | parent; }
} dtype;

class BFSMultiVertex;
class BFSEdge;
class	CProxy_BFSMultiVertex;
typedef GraphLib::Graph<
	BFSMultiVertex,									// Vertex
	//std::pair<CmiUInt8, BFSEdge>,   // Edge
	BFSEdge,   // Edge
	CProxy_BFSMultiVertex,
	GraphLib::TransportType::/*Tram*/Charm
	> BFSGraph;

#include "tram_multivertex_parallel_search.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, long long, BFSMultiVertex,
                         SimpleMeshRouter> aggregator;
const int numMsgsBuffered = 1024;

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSVertex {
	private:
		CmiUInt8 thisIndex;
		std::vector<BFSEdge> adjlist;
		bool visited;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : thisIndex(-1), visited(false), 
			parent(-1), numScannedEdges(0) {}
		BFSVertex(CmiUInt8 idx) : thisIndex(idx), visited(false), 
			parent(-1), numScannedEdges(0) {}
		void connectVertex(const BFSEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(BFSMultiVertex & multiVertex, const dtype & data); 
		void verify(BFSMultiVertex & multiVertex); 
		void check(); 
		const CmiUInt8 getScannedEdgesNum() { return numScannedEdges; }
		const CmiUInt8 getScannedVertexNum() { return (visited ? 1 : 0); }
};

class BFSMultiVertex : public CBase_BFSMultiVertex {
	private:
		std::vector<BFSVertex> vertices;
		std::deque<dtype> q;

	public:
		BFSMultiVertex() {
			//vertices.assign(N / ckGetArraySize(), BFSVertex());
			for(CmiUInt8 i = 0; i < N / ckGetArraySize(); i++)
				vertices.push_back(BFSVertex(getBase() + i));
		}
		/*BFSMultiVertex(CmiUInt8 n) {
			vertices.assign(n, BFSVertex());
		}*/

		void connectVertex(const std::pair<CmiUInt8, BFSEdge> & edge) {
			vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
		}

		void process(const std::pair<CmiUInt8, BFSEdge > & edge) {
			connectVertex(edge);
		}

		BFSMultiVertex(CkMigrateMessage *msg) {}

		inline std::deque<dtype> & getQ() { return q; }

		void make_root(const CmiUInt8 & root) {
			process(dtype(root, root)); 
		}

		inline void process(const dtype & data) {
			q.push_back(data);
			while (!q.empty()) {
				dtype d = q.front();
				CkAssert(getBaseIndex(d.v) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(d.v)].update(*this, d);
			}
		}

		void verify() {
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				dtype d = q.front();
				CkAssert(getBaseIndex(d.v) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(d.v)].check();
			}
		}

		void check(const CmiUInt8 & v) {
			vertices[getLocalIndex(v)].check();
		}

		void getScannedEdgesNum() {
			CmiUInt8 numScannedEdges = 0;
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				numScannedEdges += it->getScannedEdgesNum();
			contribute(sizeof(CmiUInt8), &numScannedEdges, CkReduction::sum_long,
								 CkCallback(CkReductionTarget(TestDriver, done),
														driverProxy));
		}

		void getScannedVertexNum() {
			CmiUInt8 numScannedVertices = 0;
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				numScannedVertices += it->getScannedVertexNum();
			contribute(sizeof(CmiUInt8), &numScannedVertices, CkReduction::sum_long,
								 CkCallback(CkReductionTarget(TestDriver, done),
														driverProxy));
		}

		inline bool isLocalIndex(const CmiUInt8 & gIdx) { 
			return thisIndex == (gIdx / (N / ckGetArraySize())); 
		}
		inline CmiUInt8 getBaseIndex(const CmiUInt8 & gIdx) { 
			return gIdx / (N / ckGetArraySize()); 
		}
		inline CmiUInt8 getLocalIndex(const CmiUInt8 & gIdx) { 
			return gIdx % (N / ckGetArraySize()); 
		}
		inline CmiUInt8 getBase() { 
			return thisIndex * (N / ckGetArraySize()); 
		}

		void foo() {} 
};

void BFSVertex::update(BFSMultiVertex & multiVertex, const dtype & data) {
	if (visited)
		return;

	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	visited = true;
	this->parent = data.parent;

	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	ArrayMeshStreamer<dtype, long long, BFSMultiVertex, SimpleMeshRouter>
		* localAggregator = aggregator.ckLocalBranch();

	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(dtype(it->v, thisIndex));
		else
			//thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v);
      localAggregator->insertData(dtype(it->v, thisIndex), multiVertex.getBaseIndex(it->v));
		numScannedEdges++;
	}
}

void BFSVertex::verify(BFSMultiVertex & multiVertex) {
	if (!visited)
		return;
	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(dtype(it->v, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].check(it->v);
	}
}

void BFSVertex::check() {
	CkAssert(visited);
}


class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
		Options, 
		GraphLib::VertexMapping::MultiVertex,
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
    graph = new BFSGraph(CProxy_BFSMultiVertex::ckNew(CmiNumPes()));
		// create graph generator
		generator = new Generator(*graph, opts);

    int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
    CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

    // Instantiate communication library group with a handle to the client
    aggregator =
      CProxy_ArrayMeshStreamer<dtype, long long, BFSMultiVertex, SimpleMeshRouter>
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
		srandom(1);
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
		CkPrintf("start, root=%lld\n", root);
    starttime = CkWallTimer();

		g[root / (N / CmiNumPes())].make_root(root);
    //CkCallback startCb(CkIndex_BFSMultiVertex::make_root(root), g[root / (N / CmiNumPes())]);
    CkCallback startCb(CkIndex_BFSMultiVertex::foo(), g[root / (N / CmiNumPes())]);
    CkCallback endCb(CkIndex_TestDriver::startVerificationPhase(), driverProxy);
    aggregator.init(g.ckGetArrayID(), startCb, endCb, -1, true);

		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		BFSGraph::Proxy & g = graph->getProxy();
		root = random() % N;
		CkPrintf("restart, root=%lld\n", root);
    starttime = CkWallTimer();

		g[root / (N / CmiNumPes())].make_root(root);
    //CkCallback startCb(CkIndex_BFSMultiVertex::make_root(root), g[root / (N / CmiNumPes())]);
    CkCallback startCb(CkIndex_BFSMultiVertex::foo(), g[root / (N / CmiNumPes())]);
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
		BFSGraph::Proxy & g = graph->getProxy();
		CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total, 
				N, 100.0*total/N, M, 100.0*total/M, root);

		if (total < 0.25 * N) {
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			//double gteps = 1e-9 * globalNumScannedEdges * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			//CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedEdges, (double)globalNumScannedEdges*100/M);
			//CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			//CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
			//				 gteps / CkNumPes());
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

#include "tram_multivertex_parallel_search.def.h"
