#include <GraphLib.h>
#include <common.h>
#include <sstream>

class SSSPVertex;
class SSSPEdge;
class	SSSPGraph;

#include "charm_sssp.decl.h"

class SSSPGraph : public GraphLib::Graph<
	SSSPVertex,
	SSSPEdge,
	CProxy_SSSPVertex,
	GraphLib::TransportType::Charm> {
public:
	SSSPGraph() : 
		GraphLib::Graph<
				SSSPVertex, 
				SSSPEdge,
				CProxy_SSSPVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	SSSPGraph(CmiUInt8 nVertex) : 
		GraphLib::Graph<
				SSSPVertex, 
				SSSPEdge,
				CProxy_SSSPVertex, 
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	SSSPGraph(const CProxy_SSSPVertex & g) : 
		GraphLib::Graph<
				SSSPVertex, 
				SSSPEdge,
				CProxy_SSSPVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void start(CmiUInt8 root) {
		g[root].make_root();
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		g[root].make_root();
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

struct SSSPEdge {
	CmiUInt8 v;
	double w;

	SSSPEdge() {}
	SSSPEdge(CmiUInt8 v, double w) : v(v), w(w) {}
	void pup(PUP::er &p) { 
		p | v; 
		p | w;
	}
};


class SSSPVertex : public CBase_SSSPVertex {
private:
	std::vector<SSSPEdge> adjlist;
	double weight;
	CmiUInt8 parent;

public:
  SSSPVertex() : weight(std::numeric_limits<double>::max()), parent(-1) {
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const SSSPEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const SSSPEdge & edge) {
		connectVertex(edge);
	}

  SSSPVertex(CkMigrateMessage *msg) {}

	void make_root() {
		weight = 0;
		parent = thisIndex;
		// broadcast
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(thisIndex, weight + it->w);
		}
	}

  void update(const CmiUInt8 & v, const double & w) {
		if (w < weight) {
			//CkPrintf("%d: %2.2f -> %2.2f\n", thisIndex, weight, w);

			// update current weight and parent
			weight = w;
			parent = v;

			// broadcast
			typedef typename std::vector<SSSPEdge>::iterator Iterator; 
			for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
				thisProxy[it->v].update(thisIndex, weight + it->w);
			}
		}
  }

	void getScannedVertexNum() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent].check(weight);
	}

	void check(double w) {
		CkAssert(weight < w);
	}

	void print() {
		std::stringstream ss;
		typedef typename std::vector<SSSPEdge>::iterator Iterator; 
		/*for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			ss << '(' << it->v << ',' << it->w << ')';
		}*/
		CkPrintf("%d: %.2f, %lld, %s\n", thisIndex, weight, parent, ss.str().c_str());
	}
};

typedef GraphLib::GraphGenerator<
	SSSPGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphType::Directed,
	GraphLib::GraphGeneratorType::Kronecker,
	GraphLib::TransportType::Tram> Generator;
#include "driver.C"
#include "charm_sssp.def.h"
