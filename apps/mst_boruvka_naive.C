#include <stdio.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <getopt.h>
#include "graphio.h"
#include "mst_boruvka_naive.decl.h"

/***
 * MSTVertexInputData structure is used to initialize chares with
 * vertex data. The instances of MSTVertexInputData are created in
 * graphio library and sent to chares.
 ***/
struct MSTVertexInputData {
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
 * MSTVertexId structure is used to identify vertex and thei supervertex which 
 * it belongs to.
 ***/
struct MSTVertexId { 
	uint64_t vid;
	uint64_t svid;
	MSTVertexId() {}
	MSTVertexId (uint64_t vid, uint64_t svid) :
		vid(vid), svid(svid) {}
	MSTVertexId (uint64_t vid) :
		vid(vid), svid(vid) {}
	void pup(PUP::er &p) {
		p|vid;
		p|svid;
	}	
};

/***
 * MSTEdgeS structure is used to store edges with only destination vertex 
 * specified.  
 ***/
struct MSTEdgeS {
	MSTVertexId dst;
	double weight;
	uint64_t id;
	bool local;
	MSTEdgeS() {}
	MSTEdgeS(uint64_t vid, double weight, uint64_t id) :
		dst(vid), weight (weight), id(id), local(false) {}
	void pup(PUP::er &p) {
		p|dst;
		p|weight;
		p|id;
		p|local;
	}	
};
bool operator< (const MSTEdgeS& lhs, const MSTEdgeS& rhs) { return lhs.weight < rhs.weight; }
using MSTEdge = MSTEdgeS;

/***
 * MSTEdgeS structure is used to store edges with source and destination vertices 
 * specified.  
 ***/
struct MSTEdgeD {
	MSTVertexId src;
	MSTVertexId dst;
	double weight;
	uint64_t id;
	MSTEdgeD() {}
	MSTEdgeD(MSTVertexId src, MSTVertexId dst, double weight, uint64_t id) :
		src(src), dst(dst), weight(weight), id(id) {}
	void pup(PUP::er &p) {
		p|src;
		p|dst;
		p|id;
		p|weight;
	}
};

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ CProxy_IO<MSTVertexInputData, CProxy_MSTVertex, GraphFileIOFormat::GraphHPC, 
								GeneratorType::Random, DistrMode::Block > io;
/*readonly*/ //CProxy_MSTVertex g;

/***
 * MSTVertex class is implementing Boruvka's algorithm in vertex-centric way. 
 ***/
class MSTVertex : public CBase_MSTVertex {
		private:
				std::vector<MSTEdge> edges;				// vertex edges
				uint64_t svid;										// current supervertex id
				bool svid_updated;								// shows wether svid has been updated 
				std::vector<MSTEdge *> mst_edges; // edges belonging to mst
				double minWeight;									// weight of the edge with current minimal weight 
				uint64_t minVertexId;						  // vertex identifier of the owner of the edge with current minimal weight
				std::vector<MSTEdgeS>::iterator e; // index of the edge in the edges list (all edges <= e are scanned) 
				CProxy_MSTVertex g;

				bool visited;
				bool verbose;

		public:
				MSTVertex() {
						// initialize vertex data
						svid = thisIndex;
						svid_updated = false;
						g = thisProxy;

						visited = false;
						verbose = false;
				}

				MSTVertex(CkMigrateMessage *m) {}

				/*entry*/ void init(CProxy_MSTVertex &g) { 
						//this->g = g; 
				}

				/*entry*/ void acceptVertexData (MSTVertexInputData &vdata) {
						for (uint16_t i = 0; i < vdata.edge_dsts.size(); i++) {
								edges.push_back(MSTEdgeS(vdata.edge_dsts[i], vdata.edge_weigths[i], vdata.edge_ids[i]));
						}
						std::sort(edges.begin(), edges.end());
				}

				/*entry*/void search() {

						// scan edge list for the edge with minimum weight and connecting to another suververtex 
						for (e = edges.begin(); e != edges.end(); e++) {
								if (!(e->local))
										break;	
						}

						if (e == edges.end()) {
								// if no external edges found then set 'special' values
								// to minVertexId and minWeight
								minVertexId = -1;
								minWeight = std::numeric_limits<double>::max();
						}
						else {
								// if external edges have been found choose one with minimal
								// weight and set minWeight to that value. 
								minWeight = e->weight;
								minVertexId = thisIndex;

								// broadcast local minimum (only within supervertex)
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++) {
										if (i->local)
												g[i->dst.vid].postMinWeight(thisIndex, e->weight);
								}
						}

				}

				/*entry*/void postMinWeight(uint64_t vid, double weight) {
						if (weight < minWeight) {
								minWeight = weight;
								minVertexId = vid;

								// broadcast local minimum (only within supervertex)
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++) {
										if (i->local)
												g[i->dst.vid].postMinWeight(minVertexId, minWeight);
								}
						} 
				}

				/*entry*/void propagateSuperVertexId(uint64_t src, uint64_t svid, bool connected) {

						if (svid < this->svid) {
								// if new svid less then current svid, update it
								// and propagate to its neighbors
								this->svid = svid;
								svid_updated = true;

								// broadcast svid (only within supervertex)
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
										if (i->local)
												g[i->dst.vid].propagateSuperVertexId(thisIndex, svid, true);
						} else if (this->svid < svid) {
								// bounce back 
							  // FIXME: I think that it is not needed, check it later!
								g[src].propagateSuperVertexId(thisIndex, this->svid, true);
						}

						// mark in edge list new locality
						if (!connected)
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
										if (i->dst.vid == src)
												i->local = true;
				}

				/*entry*/ void addEdgeToMST(uint64_t src, double weight) {
					for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
				//FIXME: src check should be added
						if (i->weight == weight)
							mst_edges.push_back(&(*i));
				}

				/*entry*/ void merge() {
						if (minVertexId != thisIndex)
								return;

						mainProxy.setStopFlag(false);

						// set local flag and propage supervertex id
						//CkPrintf("MSTVertex %d (%lld, pe=%d): merge by (%lld, %f)\n",thisIndex, svid, CkMyPe(), e->dst.vid, e->weight);
						e->local = true;
						g[e->dst.vid].propagateSuperVertexId(thisIndex, svid, false);

						// add edge and its counterpart edge to mst
						mst_edges.push_back(&(*e));
						g[e->dst.vid].addEdgeToMST(thisIndex, e->weight);
				}

				/*entry*/void updateLocal() {
						if (svid_updated) {
							  //int c = 0;
								// broadcast svid (only within supervertex)
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
										if (!i->local) {
												g[i->dst.vid].updateLocalInNeighbor(thisIndex, svid);
								//				c++;
										}
								//CkPrintf("MSTVertex %d: c = %d\n", thisIndex, c);
								svid_updated = false;
						}
				}

				/*entry*/void updateLocalInNeighbor(uint64_t src, uint64_t svid) {
						for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
								if (i->dst.vid == src)
										i->dst.svid = svid;

						if (this->svid == svid) {
								for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
										if (i->dst.vid == src) 
												i->local = true;
								g[src].updateLocalInNeighborReply(thisIndex);
						}
				}

				/*entry*/void updateLocalInNeighborReply(uint64_t src) {
						for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++)
								if (i->dst.vid == src)
										i->local = true;
				}

				/*entry*/ void gatherMSTRoots() {
					if (svid == thisIndex) 
						mainProxy.addMSTRoot(thisIndex);
				}

				/*entry*/ void gatherMST() {
						for (std::vector<MSTEdgeS *>::iterator i = mst_edges.begin(); i != mst_edges.end(); i++)
							g[(*i)->dst.vid].gatherMST(thisIndex, (*i)->weight, (*i)->id);
						visited = true;
				}

				/*entry*/ void gatherMST(uint64_t src, double weight, uint64_t id) {
					if (!visited) {
						//CkPrintf("MSTVertex %d (%lld): MSTEdge (%lld, %d, %f)\n",thisIndex, svid, src, thisIndex, weight);
						MSTVertexId s(src, svid);
						MSTVertexId d(thisIndex, svid);
						mainProxy.acceptTreeEdge(svid, MSTEdgeD(s, d, weight, id));
						for (std::vector<MSTEdgeS *>::iterator i = mst_edges.begin(); i != mst_edges.end(); i++)
							g[(*i)->dst.vid].gatherMST(thisIndex, (*i)->weight, (*i)->id);
						visited = true;
					}
				}

				/*entry*/ void print() {
						std::stringstream s;
						for (std::vector<MSTEdgeS>::iterator i = edges.begin(); i != edges.end(); i++) {
								s << "(" << i->dst.vid << "/" << i->dst.svid << "," << i->weight << "," << i->local << ") ";
						}
						if (thisIndex == svid)
							CkPrintf("MSTVertex %d* (%lld, pe=%d), Edge list (sorted): %s\n", thisIndex, svid, CkMyPe(),
									s.str().c_str());
						else
							CkPrintf("MSTVertex %d (%lld, pe=%d), Edge list (sorted): %s\n", thisIndex, svid, CkMyPe(),
									s.str().c_str());
						//CkPrintf("MSTVertex %d (%lld), Edge list (sorted): %s, Min: %lld/%f\n", thisIndex, svid, 
						//		s.str().c_str(), minVertexId, minWeight);
				}
};

class Main : public CBase_Main {
	private:
		std::string ifile;
		std::string ofile;

		uint64_t nVertex;
		uint64_t nEdges;
		CProxy_MSTVertex g;

		int icount;
		bool stopFlag;

		class MSTTree {
				uint64_t treeId;
			public: //FIXME
				std::vector<MSTEdgeD> edges;
			public:
				MSTTree(uint64_t treeId) : treeId(treeId) {}
				uint64_t getId() { return treeId; }
				void addEdge(MSTEdgeD &edge) {
					edges.push_back(edge);
				}
				void print() {
					CkPrintf("Tree %lld:\n", treeId);
					for (std::vector<struct MSTEdgeD>::iterator i = edges.begin(); i != edges.end(); i++)
						CkPrintf("(%lld, %lld, %f)\n", i->src.vid, i->dst.vid, i->weight);
				}
				uint64_t size() { return edges.size(); }
				//operator [] 
		};
		std::vector<MSTTree> trees;

		std::vector<uint64_t> roots;
		int gatherMST_tree;

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

				CkPrintf("Running MST on %d processors\n", CkNumPes());

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
			g = CProxy_MSTVertex::ckNew(nVertex);

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
			//mainProxy.print(CkCallbackResumeThread());
			CkPrintf("Start Boruvka iteration: %d\n", icount++);

			if (!icount)
				startt = CkWallTimer();

			if (!stopFlag) {
				stopFlag = true;
				searchStart();
			}
			else {
				endt = CkWallTimer();
				CkPrintf("Boruvka's completed:\n ");
				CkPrintf("\t%d iterations\n", icount);
				CkPrintf("\t%f secs, %f.4 MTEPS \n", endt-startt, ((double)nEdges)/(1000000 *(endt-startt)));
				if (verify || verbose)
					gatherMSTRoots();
				else
					CkExit();
			}
		}

		void searchStart() {
			//CkPrintf("Search start\n");
			//mainProxy.print(CkCallbackResumeThread());
			CkStartQD(CkIndex_Main::searchDone(), &thishandle);
			g.search();
		}

		/*entry*/ void searchDone() {
			//CkPrintf("Search is done\n");
			mergeStart();
		}

		void mergeStart() {
			CkStartQD(CkIndex_Main::mergeDone(), &thishandle);
			g.merge();
		}

		/*entry*/ void mergeDone() {
			//CkPrintf("Merge is done\n");
			updateLocalStart();
		}

		void updateLocalStart() {
			g.updateLocal();
			CkStartQD(CkIndex_Main::updateLocalDone(), &thishandle);
		} 

		/*entry*/ void updateLocalDone() {
			//CkPrintf("UpdateLocal is done\n");
			//g.print();
			CkStartQD(CkIndex_Main::startIteration(), &thishandle);
		}

		/*entry*/void setStopFlag(bool flag) {
			stopFlag = flag;
		}

		/*entry*/ void gatherMSTRoots() {
			CkPrintf("Adding roots...\n");
			g.gatherMSTRoots();
			CkStartQD(CkIndex_Main::gatherMST(), &thishandle);
		}
		/*entry*/ void gatherMST() {
				gatherMST_tree = 0;
				gatherMST_impl();
				CkStartQD(CkIndex_Main::done(), &thishandle);
		}

		/*entry*/ void gatherMST_impl() {
			int &i = gatherMST_tree;
			if (i < roots.size()) {
				CkPrintf("Print Minimum Spanning Tree: root = %lld\n", roots[i]);
				g[roots[i++]].gatherMST();
				CkStartQD(CkIndex_Main::gatherMST_impl(), &thishandle);
			} else {
				CkPrintf("Print Minimum Spanning Trees (collected)\n");
				std::vector<MSTTree>::iterator t;
				for (t = trees.begin(); t != trees.end(); t++) {
					t->print();
				}
				//exit();
			}
		}

		/*entry*/ void done() {
			forest_t trees_output;
			convertToGraphHPC(&trees_output);

			if(!ofile.empty())
				GraphHPC_GraphFileIO<MSTVertexInputData>::storeTreeToFile(ofile, &trees_output);
			else {
				CkError("output file is not set");
				CkExit();
			}
			CkStartQD(CkIndex_Main::exit(), &thishandle);
		}

		/*entry*/ void print(CkCallback &cb) {
			g.print();
			CkStartQD(cb);
		}

		/*entry*/ void exit() {
			CkExit();
		}

		/*entry*/ void addMSTRoot(uint64_t root) { 
			roots.push_back(root); 
			trees.push_back(MSTTree(root));
		}

		/*entry*/ void acceptTreeEdge(uint64_t treeId, MSTEdgeD &edge) {
			std::vector<MSTTree>::iterator t;
			for (t = trees.begin(); t != trees.end(); t++) {
				if (t->getId() == treeId)
					break;
			}
			t->addEdge(edge);
		}

		void convertToGraphHPC(forest_t *forest) {
			forest->p_edge_list = (edge_id_t *)malloc(trees.size()*2 * sizeof(edge_id_t));
			edge_id_t number_of_edges = 0;
			for (vertex_id_t i = 0; i < trees.size(); i++) number_of_edges += trees[i].size();
			forest->edge_id = (edge_id_t *)malloc(number_of_edges * sizeof(edge_id_t));
			forest->p_edge_list[0] = 0;
			forest->p_edge_list[1] = trees[0].size();
			for (vertex_id_t i = 1; i < trees.size(); i++) {
				forest->p_edge_list[2*i] = forest->p_edge_list[2*i-1];
				forest->p_edge_list[2*i +1] = forest->p_edge_list[2*i-1] + trees[i].size();
			}
			int k = 0;
			for (vertex_id_t i = 0; i < trees.size(); i++) {
				for (edge_id_t j = 0; j < trees[i].size(); j++) {
					forest->edge_id[k] = trees[i].edges[j].id;
					k++;
				}
			}

			forest->numTrees = trees.size();
			forest->numEdges = number_of_edges;
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

#include "mst_boruvka_naive.def.h"
