#include <vector>
#include <sstream>

#include "InputOutput.h"
#include "test.decl.h"

struct VertexData {
	std::vector<uint64_t> edges;
	std::vector<double> edge_weigths;
	public:
		void pup(PUP::er &p) {
			p|edges;
			p|edge_weigths;
		}	
};

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ CProxy_Vertex vertexProxy;
/*readonly*/ CProxy_IO<VertexData, CProxy_Vertex, GraphFileIOFormat::GraphHPC /*FIXME*/, DistrMode::Block > ioProxy;

/*readonly*/ int nScale = 5;
/*readonly*/ int nK = 4;

/***
 *  Vertex class. 
 ***/
class Vertex : public CBase_Vertex 
{
private:

	struct Edge 
	{
		uint64_t dst;	// destination vertex id
		double weight;
		Edge(uint64_t dst, double weight) : dst(dst), weight(weight) {}
	};

	std::vector<Edge> edges;		// vertex edges
	CProxy_Vertex g;

public:
	Vertex() 
	{
		CkPrintf("%d : hello\n", thisIndex);
	}
	Vertex(CkMigrateMessage *m) {}

	/*entry*/ void acceptVertexData (VertexData &vdata) {
		CkPrintf("%d : acceptVertexData called\n", thisIndex);
		for (uint16_t i = 0; i < vdata.edges.size(); i++) {
			edges.push_back(Edge(vdata.edges[i], vdata.edge_weigths[i]));
		}
		print();
	}

	/*entry*/ void print() {
		std::stringstream ss;
		for (uint16_t i = 0; i < edges.size(); i++) {
			if (i) ss << ", ";
			ss << "(" << edges[i].dst << ", " << edges[i].weight << ")";
		}
		CkPrintf("Vertex[%d] : %s\n", thisIndex, ss.str().c_str());
	}
};


/*mainchare*/
class Main : public CBase_Main
{
private:
public:
  Main(CkArgMsg* m)
  {
	  CkPrintf("Running on %d processors for graph of scale %d \n",
			  CkNumPes(), nScale);
	  mainProxy = thisProxy;

	  // create io group
	  //ioProxy = CProxy_IO<VertexData, GraphFileIOFormat::Generator, DistrMode::Block>::ckNew("test.dat");
	  ioProxy = CProxy_IO<VertexData, CProxy_Vertex, GraphFileIOFormat::GraphHPC /*FIXME*/, DistrMode::Block>::ckNew(nScale, nK);

	  //CProxy_Foo<VertexData, GraphFileIOFormat::GraphHPC> foo = CProxy_Foo<VertexData, GraphFileIOFormat::GraphHPC>::ckNew();

	  // create vertex array (graph)
	  vertexProxy = CProxy_Vertex::ckNew(1<<nScale);

	  ioProxy.init(vertexProxy);

	  // I dont know how to stop computation otherwise
	  CkStartQD(CkIndex_Main::done(), &thishandle);
  };

  void do_min(CProxy_Vertex &g) 
  {
	  //g.print();
	  //g.compute();
  }

  void done()
  {
	  CkPrintf("Start loading graph data and initializing chares\n");
	  ioProxy.loadGraphToChares();
	  //g.print();
	  //CkPrintf("All done\n");
	  //CkExit();
  };
};

#include "test.def.h"
