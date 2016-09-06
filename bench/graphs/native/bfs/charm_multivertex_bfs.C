#include <GraphLib.h>
#include <common.h>
#include <deque>

typedef struct __dtype {
	CmiUInt8 v;
	int level;
	CmiUInt8 parent;
	__dtype() {}
	__dtype(CmiUInt8 v, int level, CmiUInt8 parent) : v(v), level(level), parent(parent) {}
	void pup(PUP::er & p) { p | v; p | level; p | parent; }
} dtype;

class BFSMultiVertex;
class BFSEdge;
class	BFSGraph;

#include "charm_multivertex_bfs.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

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
		int level;
		CmiUInt8 parent;

	public:
		BFSVertex() : thisIndex(-1), level(-1), 
			parent(-1) {}
		BFSVertex(CmiUInt8 idx) : thisIndex(idx), level(-1), 
			parent(-1) {}
		void connectVertex(const BFSEdge & edge) {
			adjlist.push_back(edge);
		}
		//...
		void update(BFSMultiVertex & multiVertex, int level, CmiUInt8 parent); 
		void verify(BFSMultiVertex & multiVertex); 
		void check(int level); 
		const CmiUInt8 getScannedVertexNum() { return (level >= 0 ? 1 : 0); }
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
			CkAssert(getBaseIndex(root) == thisIndex);
			update(root, 0, root);
		}

		void update(const CmiUInt8 & v, int level, CmiUInt8 parent) {

			q.push_back(dtype(v, level, parent));
			while (!q.empty()) {
				dtype data = q.front();
				CkAssert(getBaseIndex(data.v) == thisIndex);

				q.pop_front();
				vertices[getLocalIndex(data.v)].update(*this, data.level, data.parent);
			}
		}

		void verify() {
			typedef std::vector<BFSVertex>::iterator Iterator;
			for (Iterator it = vertices.begin(); it != vertices.end(); it++) 
				it->verify(*this);
			while (!q.empty()) {
				dtype data = q.front();
				CkAssert(getBaseIndex(data.v) == thisIndex);
				q.pop_front();
				vertices[getLocalIndex(data.v)].check(data.level);
			}
		}

		void check(const CmiUInt8 & v, int level) {
			vertices[getLocalIndex(v)].check(level);
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
};

void BFSVertex::update(BFSMultiVertex & multiVertex, int level, CmiUInt8 parent) {
	if ((this->level >= 0) && (this->level <= level + 1))
		return;

	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 
	this->level = level + 1;
	this->parent = parent;

	typedef typename std::vector<BFSEdge>::iterator Iterator; 
	for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
		if (multiVertex.isLocalIndex(it->v))
			q.push_back(dtype(it->v, this->level, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(it->v)].update(it->v, this->level, thisIndex);
	}
}

void BFSVertex::verify(BFSMultiVertex & multiVertex) {
	if (level < 0)
		return;

	std::deque<dtype> & q = multiVertex.getQ();
	CProxy_BFSMultiVertex & thisProxy = multiVertex.thisProxy; 

	if ((parent != -1) && (parent != thisIndex))
		if (multiVertex.isLocalIndex(parent))
			q.push_back(dtype(parent, level, thisIndex));
		else
			thisProxy[multiVertex.getBaseIndex(parent)].check(parent, level);
}

void BFSVertex::check(int level) {
	CkAssert(this->level == level - 1);
}

typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::MultiVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"
#include "charm_multivertex_bfs.def.h"
