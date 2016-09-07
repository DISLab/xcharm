#include <GraphLib.h>
#include <common.h>
#include <deque>

class BFSMultiVertex;
class BFSEdge;
class	BFSGraph;

#include "charm_multivertex_parallel_search.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSGraph : public GraphLib::Graph<
	BFSMultiVertex,
	BFSEdge,
	CProxy_BFSMultiVertex,
	GraphLib::TransportType::Charm> {
public:
	BFSGraph() : 
		GraphLib::Graph<
				BFSMultiVertex, 
				BFSEdge,
				CProxy_BFSMultiVertex, 
				GraphLib::TransportType::Charm >()
		{}
	BFSGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				BFSMultiVertex, 
				BFSEdge,
				CProxy_BFSMultiVertex, 
				GraphLib::TransportType::Charm >(CmiNumPes())	
		{}
	void start(CmiUInt8 root) {
		g[root / (N / CmiNumPes())].make_root(root);
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		g[root / (N / CmiNumPes())].make_root(root);
		CkStartQD(cb);
	}
	void getScannedVertexNum() {
		g.getScannedVertexNum();
	}
	void verify() {
		g.verify();
	}
};

class BFSVertex {
	private:
		std::vector<BFSEdge> adjlist;
		bool visited;
		CmiUInt8 parent;
		CmiUInt8 numScannedEdges;

	public:
		BFSVertex() : visited(false), parent(-1), numScannedEdges(0) {}
		void connectVertex(const BFSEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(BFSMultiVertex & multiVertex); 
		void verify(BFSMultiVertex & multiVertex); 
		void check(); 
		const CmiUInt8 getScannedEdgesNum() { return numScannedEdges; }
		const CmiUInt8 getScannedVertexNum() { return (visited ? 1 : 0); }
};

class BFSMultiVertex : public CBase_BFSMultiVertex {
	private:
		std::vector<BFSVertex> vertices;
		std::deque<CmiUInt8> q;

	public:
		BFSMultiVertex() {
			vertices.assign(N / ckGetArraySize(), BFSVertex());
		}
		BFSMultiVertex(CmiUInt8 n) {
			vertices.assign(n, BFSVertex());
		}

		void connectVertex(const std::pair<CmiUInt8, BFSEdge> & edge) {
			vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
		}

		void process(const std::pair<CmiUInt8, BFSEdge > & edge) {
			connectVertex(edge);
		}

		BFSMultiVertex(CkMigrateMessage *msg) {}

		inline std::deque<CmiUInt8> & getQ() { return q; }

		void make_root(const CmiUInt8 & v) {
			update(v);
		}

		void update(const CmiUInt8 & v) {

			q.push_back(v);
			while (!q.empty()) {
				CmiUInt8 u = q.front();
				CkAssert(getBaseIndex(u) == thisIndex);

				q.pop_front();
				vertices[getLocalIndex(u)].update(*this);
			}
		}

		void verify() {
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				CmiUInt8 u = q.front();
				CkAssert(getBaseIndex(u) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(u)].check();
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
};

void BFSVertex::update(BFSMultiVertex & multiVertex) {
	if (visited)
		return;

	std::deque<CmiUInt8> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	visited = true;

	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(it->v);
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v);
		numScannedEdges++;
	}
}

void BFSVertex::verify(BFSMultiVertex & multiVertex) {
	if (!visited)
		return;
	std::deque<CmiUInt8> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(it->v);
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].check(it->v);
	}
}

void BFSVertex::check() {
	CkAssert(visited);
}

typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::MultiVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"
#include "charm_multivertex_parallel_search.def.h"
