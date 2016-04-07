#include <stdio.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <getopt.h>
#include "graphio.h"
#include "sssp_dejkstra.decl.h"

/***
 * SSSPVertexInputData structure is used to initialize chares with
 * vertex data. The instances of SSSPVertexInputData are created in
 * graphio library and sent to chares.
 ***/
struct SSSPVertexInputData {
	std::vector<uint64_t> edge_dsts;
	std::vector<uint64_t> edge_ids;
	std::vector<double> edge_weigths;
	public:
		void pup(PUP::er &p) {
			p|edge_dsts;
			p|edge_ids;
			p|edge_weigths;
		}	
};

/***
 * SSSPVertexId structure is used to identify vertex and thei supervertex which 
 * it belongs to.
 ***/
struct SSSPVertexId { 
	uint64_t vid;
	uint64_t svid;
	SSSPVertexId() {}
	SSSPVertexId (uint64_t vid, uint64_t svid) :
		vid(vid), svid(svid) {}
	SSSPVertexId (uint64_t vid) :
		vid(vid), svid(vid) {}
	void pup(PUP::er &p) {
		p|vid;
		p|svid;
	}	
};

/***
 * SSSPEdgeS structure is used to store edges with only destination vertex 
 * specified.  
 ***/
struct SSSPEdgeS {
	SSSPVertexId dst;
	double weight;
	uint64_t id;
	bool local;
	SSSPEdgeS() {}
	SSSPEdgeS(uint64_t vid, double weight, uint64_t id) :
		dst(vid), weight (weight), id(id), local(false) {}
	void pup(PUP::er &p) {
		p|dst;
		p|weight;
		p|id;
		p|local;
	}	
};
bool operator< (const SSSPEdgeS& lhs, const SSSPEdgeS& rhs) { return lhs.weight < rhs.weight; }
using SSSPEdge = SSSPEdgeS;

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ CProxy_IO<SSSPVertexInputData, CProxy_SSSPVertex, GraphFileIOFormat::GraphHPC, 
								GeneratorType::Random, DistrMode::Block > io;

/***
 * SSSPVertex class is implementing Dejkstra algorithm in vertex-centric way. 
 ***/
class SSSPVertex : public CBase_SSSPVertex {
		private:
				std::vector<SSSPEdge> edges;				// vertex edges
				CProxy_SSSPVertex g;
				bool visited;
				bool verbose;

		public:
				SSSPVertex() {
						// initialize vertex data
						svid = thisIndex;
						svid_updated = false;
						g = thisProxy;

						visited = false;
						verbose = false;
				}

				SSSPVertex(CkMigrateMessage *m) {}

				/*entry*/ void init(CProxy_SSSPVertex &g) { 
						//this->g = g; 
				}

				/*entry*/ void acceptVertexData (SSSPVertexInputData &vdata) {
						for (uint16_t i = 0; i < vdata.edge_dsts.size(); i++) {
								edges.push_back(SSSPEdgeS(vdata.edge_dsts[i], vdata.edge_weigths[i], vdata.edge_ids[i]));
						}
						std::sort(edges.begin(), edges.end());
				}
};

class Main : public CBase_Main {
	private:
		std::string ifile;
		std::string ofile;

		uint64_t nVertex;
		uint64_t nEdges;
		CProxy_SSSPVertex g;

		int icount;
		bool stopFlag;

		double startt;
		double endt;

		bool verify;
		bool verbose;

	public:
		Main(CkArgMsg* m) : icount(0), verify(false), verbose(false) {
				mainProxy = thisProxy;

				parseCommandOptions(m->argc, m->argv);
				//printCommandOptions();
				//CkExit();

				CkPrintf("Running SSSP on %d processors\n", CkNumPes());

				// create io chares
				//io = CProxy_IO<MSTVertexInputData, CProxy_MSTVertex, GraphFileIOFormat::GraphHPC, 
				//	 GeneratorType::Random, DistrMode::Block>::ckNew(nScale, nK); 

				if (!ifile.empty())
					io = CProxy_IO<MSTVertexInputData, CProxy_MSTVertex, GraphFileIOFormat::GraphHPC, 
						 GeneratorType::Random, DistrMode::Block>::ckNew(ifile); 
				else {
					CkError("input file is not set");
					CkExit();
				}

				// request graph properties (size)
				CkCallback cb(CkIndex_Main::recvGraphProps(NULL), mainProxy);
				io.ckLocalBranch()->reqGraphProps(cb);

				CkStartQD(CkIndex_Main::init(), &thishandle);
		};

		/*enbrty*/ void recvGraphProps(GraphPropsMsg *m) {
			CkPrintf("recvGraphProps  called: %lld\n", m->nVertex);
			nVertex = m->nVertex;
			nEdges= m->nEdges;
		}

		/*entry*/void init() {
			// create chare array
			g = CProxy_SSSPVertex::ckNew(nVertex);

			// make all necessare initilizations
			io.init(g);

			// load graph after initialization is complete
			CkStartQD(CkIndex_Main::loadGraph(), &thishandle);
		}

		/*entry*/void loadGraph() {
			// load graph to chare array
			io.loadGraphToChares();

			// start mst computation after graph is loaded to chares
			CkStartQD(CkIndex_Main::startIteration(), &thishandle);
		}

		/*entry*/void startIteration() {
		}

		void parseCommandOptions(int argc, char **argv) {
			while (1) {
				int option_index = 0;
				static struct option long_options[] = {
					{"in", 		1, 0, 'i'},
					{"out", 		1, 0, 'o'},
					{"verify", 		0, 0, 'v'},
					{"verbose", 		0, 0, 'e'},
					{"generate", 		0, 0, 'g'},
					{"help", 		0, 0, 'h'},
					{0,0,0,0}
				};

				char c = getopt_long(argc, argv, "i:o:veg",
						long_options, &option_index);
				if (c == -1)
					break;

				switch (c) {
					case 'i':
						ifile = optarg;
						break;
					case 'o':
						ofile = optarg;
						break;
					case 'v':
						verify = true;
						break;
					case 'e':
						verbose = true;
						break;
					case 'h':
						CkPrintf("Usage:\n");
						CkPrintf("   mst_boruvka_naive [OPTIONS]\n\n");
						CkPrintf("Options:\n");
						CkPrintf("   --in <INFILE> 			         \tspecifies file with input graph                                     \n");
						CkPrintf("   --out <INFILE>			         \tspecifies file for output                                           \n");
					default:
						break;
				}

			}
		}

		void printCommandOptions() {

			CkPrintf("Command options:\n");
			CkPrintf("\tifile = %s\n", ifile.c_str());
			CkPrintf("\tofile = %s\n", ofile.c_str());
		}
};

#include "sssp_dejkstra.def.h"
