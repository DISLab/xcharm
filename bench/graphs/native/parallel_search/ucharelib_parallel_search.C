#include <uChareLib.h>
#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class BFSGraph;

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

#include "ucharelib_parallel_search.decl.h"

class BFSGraph : public GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_uChare_BFSVertex,
	GraphLib::TransportType::Charm> {
public:
	BFSGraph() : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_uChare_BFSVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	BFSGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_uChare_BFSVertex, 
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	BFSGraph(const CProxy_uChare_BFSVertex & g) : 
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_uChare_BFSVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void start(CmiUInt8 root) {
		g[root]->make_root();
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		g[root]->make_root();
		CkStartQD(cb);
	}
	void getScannedVertexNum() {
		g.getScannedVertexNum();
	}
	void verify() {
		g.verify();
	}
};

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
			//contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
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


typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphGeneratorType::RMAT,
	GraphLib::TransportType::Tram> Generator;

#include "driver.C"

#include "ucharelib_parallel_search.def.h"
