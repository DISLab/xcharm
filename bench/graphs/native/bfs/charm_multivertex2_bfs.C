#include <GraphLib.h>
#include <common.h>

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

#include "charm_multivertex2_bfs.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

class BFSGraph : public GraphLib::Graph<
	BFSMultiVertex,
	BFSEdge,
	CProxy_BFSMultiVertex,
	GraphLib::TransportType::Charm> {
public:
	bool directed;
public:
	BFSGraph(bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BFSMultiVertex, 
				BFSEdge,
				CProxy_BFSMultiVertex, 
				GraphLib::TransportType::Charm >()
		{}
	BFSGraph(CmiUInt8 nVertex, bool directed = true) : 
	  directed(directed),	
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
	static CmiUInt4 base(CmiUInt8 v) { return v / (N / CmiNumPes()); }
};

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSMultiVertex : public CBase_BFSMultiVertex {
	private:
		typedef std::vector<BFSEdge> EdgeList;
		std::vector<int> levels;
		std::vector<CmiUInt8> parents;
		std::vector<EdgeList> edges;

	public:
		BFSMultiVertex() {
			parents.assign(N / ckGetArraySize(), -1);
			levels.assign(N / ckGetArraySize(), -1);
			edges.assign(N / ckGetArraySize(), EdgeList());
		}
		/*BFSMultiVertex(CmiUInt8 n) {
			vertices.assign(n, BFSVertex());
		}*/

		void connectVertex(const std::pair<CmiUInt8, BFSEdge> & edge) {
			//vertices[getLocalIndex(edge.first)].connectVertex(edge.second);
			edges[getLocalIndex(edge.first)].push_back(edge.second);
		}

		void process(const std::pair<CmiUInt8, BFSEdge > & edge) {
			connectVertex(edge);
		}

		BFSMultiVertex(CkMigrateMessage *msg) {}

		void make_root(const CmiUInt8 & root) {
			CkPrintf("%lld: make_root\n", root);
			CkAssert(getBaseIndex(root) == thisIndex);
			update(root, 0, root);
		}

		void update(const CmiUInt8 & v, int level, CmiUInt8 parent) {
			const CmiUInt8 localIndex = getLocalIndex(v);
			int & l = levels[localIndex];
			if (l < 0 || level < l) {
				CmiUInt8 & p = parents[localIndex];
				EdgeList & edgelist = edges[localIndex];

				p = parent;
				l = level;
				typedef typename std::vector<BFSEdge>::iterator Iterator; 
				for (Iterator it = edgelist.begin(); it != edgelist.end(); it++) {
					thisProxy[getBaseIndex(it->v)].update(it->v, level + 1, v);
				}

			}
		}

		void verify() {
			for (CmiUInt8 localIndex = 0; localIndex < getLocalSize(); localIndex++) {
				int & level = levels[localIndex];
				CmiUInt8 & parent = parents[localIndex];
				EdgeList & edgelist = edges[localIndex];
				if ((parent != -1) && (parent != getGlobalIndex(thisIndex, localIndex)))
					thisProxy[getBaseIndex(parent)].check(parent, level - 1);
			}
		}

		void check(const CmiUInt8 & v, int level) {
			CkAssert(getBaseIndex(v) == thisIndex);
			const CmiUInt8 localIndex = getLocalIndex(v);
			CkAssert(levels[localIndex] == level);
		}

		void getScannedVertexNum() {
			CmiUInt8 numScannedVertices = 0;
			for (CmiUInt8 localIndex = 0; localIndex < getLocalSize(); localIndex++) {
				CmiUInt8 & parent = parents[localIndex];
				numScannedVertices += (parent == -1) ? 0 : 1;
			}
			contribute(sizeof(CmiUInt8), &numScannedVertices, CkReduction::sum_long,
								 CkCallback(CkReductionTarget(TestDriver, done),
														driverProxy));
		}

		inline CmiUInt8 getLocalSize() { 
			return (N / ckGetArraySize()); 
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
		inline CmiUInt8 getGlobalIndex(const CmiUInt8 & baseIdx, const CmiUInt8 & localIdx) { 
			return baseIdx * getLocalSize() + localIdx;
		}
};


typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::MultiVertex,
	GraphLib::GraphGeneratorType::GRAPH,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"
#include "charm_multivertex2_bfs.def.h"
