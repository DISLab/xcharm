#include <GraphLib.h>
#include <common.h>

class CCVertex;
class CCEdge;
class CCGraph;

#include "charm_cc.decl.h"

class CCGraph : public GraphLib::Graph<
	CCVertex,
	CCEdge,
	CProxy_CCVertex,
	GraphLib::TransportType::Charm> {
public:
	CCGraph() : 
		GraphLib::Graph<
				CCVertex, 
				CCEdge,
				CProxy_CCVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	CCGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				CCVertex, 
				CCEdge,
				CProxy_CCVertex, 
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	CCGraph(const CProxy_CCVertex & g) : 
		GraphLib::Graph<
				CCVertex, 
				CCEdge,
				CProxy_CCVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void start() {
		g.start();
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

struct CCEdge {
	CmiUInt8 v;

	CCEdge() {}
	CCEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class CCVertex : public CBase_CCVertex {
private:
	std::vector<CCEdge> adjlist;
	CmiUInt8 componentId;

public:
  CCVertex() : componentId(thisIndex) {
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const CCEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const CCEdge & edge) {
		connectVertex(edge);
	}

  CCVertex(CkMigrateMessage *msg) {}

	void start() {
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			if (it->v != thisIndex)
				thisProxy[it->v].update(componentId);
		}
	}

	void update(const CmiUInt8 & c) {
		if (c < componentId) {
			// update current componentId
			componentId = c;
			// broadcast
			typedef typename std::vector<CCEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v].update(componentId);
			}
		}
  }

	void getScannedVertexNum() {
		CmiUInt8 c = 1 /*(parent == -1 ? 0 : 1)*/;
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void verify() {
		typedef typename std::vector<CCEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) 
			thisProxy[it->v].check(componentId);
	}

	void check(const CmiUInt8 & c) {
		CkAssert(componentId == c);
	}
};

typedef GraphLib::GraphGenerator<
	CCGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "charm_cc.def.h"
