#include <stdio.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <cmath>
#include "graphio/graphio.h"
#include "cd_louvain_naive.decl.h"
#include <getopt.h>

#if defined(GRAPHHPC_CONTEST)
//#include "defs.h"
#endif

// delta modularity calculcation function
inline double deltaFunction (double w, double m, double wa_tot, double wb_tot) {
	return 
		w / m + 1 / (m * m) *
		   (wa_tot * wa_tot + wb_tot * wb_tot - (wa_tot + wb_tot - w) * (wa_tot + wb_tot - w));
}

/*readonly*/ int nScale = 16;
/*readonly*/ int nK = 4;

typedef uint64_t EdgeDiscriptor; 

// Vertex data structure used for initialization phase
struct VertexData {
	//std::vector<EdgeDiscriptor> edges;
	std::vector<double> edge_weigths;

/*FIXME*/
	std::vector<uint64_t> edge_dsts;
	std::vector<uint64_t> edge_ids;
/*******/

	public:
		void pup(PUP::er &p) {
			//FIXME:!
			p|edge_dsts;
			p|edge_weigths;
			p|edge_ids;
		}	
};

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ CProxy_IO<VertexData, CProxy_CDVertex, GraphFileIOFormat::GraphHPC /*FIXME*/, GeneratorType::Random, DistrMode::Block > io;

/****
 * Communities... 
 ***/
class CDComm : public CBase_CDComm {
/*readonly*/ CProxy_CDVertex g;
/*readonly*/ CProxy_CDComm c;

	typedef std::map<uint64_t, std::map<uint64_t, double> > CommTable;
	typedef std::map<std::pair<int64_t,uint64_t>, double > DeltaBuffer;
	typedef std::map<uint64_t, double > TransReqBuffer;
	CommTable table;
	DeltaBuffer deltas;
	TransReqBuffer transReqBuffer;
	double sigma_in, sigma_tot;
	double m;

	double maxdelta;
	uint64_t comm, vert;

	double sumdelta;

	double localModularity;

	public:
		CDComm() : sigma_in(0), sigma_tot(0), maxdelta(0), sumdelta(0), localModularity(0) {}
		CDComm(CkMigrateMessage *m) {}

		void init(CProxy_CDVertex &g, CProxy_CDComm &c) { 
			this->g = g; 
			this->c = c;
		}

		void init(double &m) {
			this->m = m;
		}

		void updateVertexCommunityConnections(uint64_t s, std::map<uint64_t, double> newval) {
			//CkPrintf("CDComm %d: called from %lld\n", thisIndex, s);

			typedef std::map<uint64_t, double>::iterator Iterator;
			std::map<uint64_t, double> &oldval = table[s];

			double old_sigma_in = sigma_in;
			double old_sigma_tot = sigma_tot;

			for (Iterator i = oldval.begin(); i != oldval.end(); i++) { 
				if (i->first == thisIndex)
					sigma_in -= i->second;
				sigma_tot -= i->second;	
			}

			table[s] = newval;

			typedef std::map<uint64_t, double>::iterator Iterator;
			for (Iterator i = newval.begin(); i != newval.end(); i++) { 
				if (i->first == thisIndex)
					sigma_in += i->second;
				sigma_tot += i->second;	
			}

			//CkPrintf("CDComm %d: called from %lld, delta sigma_in = %f, delta sigma_tot = %f\n", 
			//		thisIndex, s, sigma_in - old_sigma_in, sigma_tot - old_sigma_tot);
		}

		void calculateModularity() {
			maxdelta = 0;
			sumdelta = 0;
			transReqBuffer.clear();
			deltas.clear();


		  typedef std::map<uint64_t, std::map<uint64_t, double> >::iterator IteratorI;	
			typedef std::map<uint64_t, double>::iterator IteratorJ;
			double verify_sigma_in = 0, verify_sigma_tot = 0;

			double newLocalModularity = sigma_in / (/*2 **/ m) - 
				4 * sigma_tot * sigma_tot / (/*4 **/ m * m); 

			//double newLocalModularity = verify_sigma_in / m - 
			//	verify_sigma_tot * verify_sigma_tot / (m * m); 

			//CkPrintf("CDComm %d: localModularity %f (%f) (sigma_in = %f, sigma_tot = %f, m = %f)\n", 
			//		thisIndex, newLocalModularity, (newLocalModularity - localModularity),  sigma_in, sigma_tot, m);


			localModularity = newLocalModularity;

			//print();

			contribute(sizeof(CmiFloat8), &localModularity, CkReduction::sum_double, 
					CkCallback(CkReductionTarget(Main, reportModularity), mainProxy));
		}

		double delta_q(double sign, double wu, double wuc, double sigma_tot, double m)
		{
			    return sign * wuc / m - (pow(sigma_tot + sign * (wu - wuc), 2) - pow(sigma_tot, 2)) / pow(m, 2);
		}

		void searchMaxDelta() {

			typedef std::map<uint64_t, std::map<uint64_t, double> >::iterator IteratorI;
			typedef std::map<uint64_t, double>::iterator IteratorJ;
		  for (IteratorI i = table.begin(); i != table.end(); i++) {
				// compute wu
				double wu = 0, wc = 0;
				for (IteratorJ j = i->second.begin(); j != i->second.end(); j++) {
					wu += j->second;
					if (j->first == thisIndex)
						wc = j->second;
				}
				for (IteratorJ j = i->second.begin(); j != i->second.end(); j++) {
					// compute first (excluding) part of delta
					double delta = delta_q(-1, 2*wu, 2*wc, sigma_tot, m);
					deltas[std::make_pair(i->first,j->first)] = delta;
					
					c[j->first].calculateDeltaSigma(thisIndex, i->first, wu, j->second);
				}
			}
		}

		/***
		 * sc -- source community
		 * u -- vertex num
		 * wu -- total weight of all edges incident to u
		 * wc -- weight of all edges from u to current community 
		 ***/
		void calculateDeltaSigma (uint64_t sc, uint64_t u, double wu, double wc) {
			double delta = delta_q(+1, 2*wu, 2*wc, sigma_tot, m);
			c[sc].calculateDeltaSigma(thisIndex, u, delta);
		}

	  void calculateDeltaSigma(uint64_t sc, uint64_t u, double delta) {
					delta += deltas[std::make_pair(u,sc)];
					//CkPrintf("CDComm %d: Vertex %lld, C%ld --> C%lld, delta = %f\n", 
					//		thisIndex, u, thisIndex, sc, delta);
					if (maxdelta < delta) {
						maxdelta = delta;
						comm = sc;
						vert = u;
					}
		}

		void filterTransitions() {

			if (maxdelta > 0) {
				//CkPrintf("CDComm %d: Vertex %lld, C%ld --> C%lld, delta = %f\n", 
				//		thisIndex, vert, thisIndex, comm, maxdelta);
				c[comm].transitionRequest(thisIndex, maxdelta);
			}
		}

		void transitionRequest(uint64_t sc, double delta) {
		//	if (sc == comm) {
				transReqBuffer[sc] = delta;
				sumdelta += delta;
				//g[vert].updateCommunity(comm);
		//	}
		}

		void transitionResponse(uint64_t sc, bool status) {
			if (status && (maxdelta >= sumdelta)) {
				//CkPrintf("CDComm %d: ***Vertex %lld, C%d --> C%lld, delta = %f (sumdelta = %f)\n", 
				//		thisIndex, vert, thisIndex, comm, maxdelta, sumdelta);
				// call vertex update community
				g[vert].updateCommunity(comm);

				typedef std::map<uint64_t, double>::iterator Iterator;
				std::map<uint64_t, double> &connections = table[vert];
				for (Iterator i = connections.begin(); i != connections.end(); i++) { 
					if (i->first == thisIndex)
						sigma_in -= i->second;
					sigma_tot -= i->second;	
				}
				table.erase(vert); //FIXME: table must be updated

			} else { 
				/*if (!status) 
					CkPrintf("CDComm %d: *** [denied, status == false] Vertex %lld, C%d --> C%lld, delta = %f (sumdelta = %f)\n", 
							thisIndex, vert, thisIndex, comm, maxdelta, sumdelta);
				else
					CkPrintf("CDComm %d: *** [denied, status == true] Vertex %lld, C%d --> C%lld, delta = %f (sumdelta = %f)\n", 
							thisIndex, vert, thisIndex, comm, maxdelta, sumdelta);
				*/
			} 
		}


		void doTransitions() {
			if (sumdelta > maxdelta) {
				typedef std::map<uint64_t, double>::iterator Iterator; 
				for (Iterator i = transReqBuffer.begin(); i != transReqBuffer.end(); i++)
					c[i->first].transitionResponse(thisIndex, true); 
			} else
				// corner case //TODO: add description
				if ((sumdelta == maxdelta) && (transReqBuffer.size() == 1)) {
					typedef std::map<uint64_t, double>::iterator Iterator; 
					for (Iterator i = transReqBuffer.begin(); i != transReqBuffer.end(); i++)
						if (i->first < thisIndex)
							c[i->first].transitionResponse(thisIndex, true); 
				}

			//maxdelta = 0;
			//sumdelta = 0;
		}

		void getTotalWeight() {
		  typedef std::map<uint64_t, std::map<uint64_t, double> >::iterator IteratorI;	
			typedef std::map<uint64_t, double>::iterator IteratorJ;
			double w = 0;
		  for (IteratorI i = table.begin(); i != table.end(); i++) 
				for (IteratorJ j = i->second.begin(); j != i->second.end(); j++) {
					w += j->second;
				}
			contribute(sizeof(CmiFloat8), &w, CkReduction::sum_double, 
					CkCallback(CkReductionTarget(Main, getTotalWeight), mainProxy));
		}

		void reconstructCommunityGraph() {
			//CkPrintf("CDComm %d: reconstructCommunityGraph called\n", thisIndex);

			// exit if community is empty
			if (table.empty())
				return;

			// otherwise calculcate community edge list
		  typedef std::map<uint64_t, std::map<uint64_t, double> >::iterator IteratorI;	
			typedef std::map<uint64_t, double>::iterator IteratorJ;

			// new community edge list
			std::map<uint64_t, double> edgelist;


		  for (IteratorI i = table.begin(); i != table.end(); i++) 
				for (IteratorJ j = i->second.begin(); j != i->second.end(); j++) {
					edgelist[j->first] += j->second;
				}

			/*std::stringstream vs;
			for (IteratorJ j = edgelist.begin(); j != edgelist.end(); j++) {
				vs << "(C" << j->first << "," << j->second << "), ";
			}

			// verify local modularity
			double verify_sigma_in = 0, verify_sigma_tot = 0;
			for (IteratorJ j = edgelist.begin(); j != edgelist.end(); j++) {
				if (j->first == thisIndex) verify_sigma_in += j->second;
				verify_sigma_tot += j->second;
			}

			double modularity = verify_sigma_in/m - pow(verify_sigma_tot/m, 2);
			CkPrintf("Comm[%d] --> local modularity %f verify_sigma_in %f, verify_sigma_tot %f, new edge list: ...\n", 
					thisIndex, modularity, verify_sigma_in, verify_sigma_tot, vs.str().c_str() );
			//CkPrintf("Comm[%d] --> verify_sigma_in %f, verify_sigma_tot %f, new edge list: %s\n", 
					//thisIndex, verify_sigma_in, verify_sigma_tot, vs.str().c_str() );
					*/
      
			// send edgelist to community "root" vertex
			uint64_t root = thisIndex/*table.begin()->first*/;
			g[root].updateEdgeList(thisIndex, edgelist);

			// clear table 
			table.clear();
			sigma_in = 0;
			sigma_tot = 0;
			maxdelta = 0;
			sumdelta = 0;
			localModularity = 0;
		}

		void print() {
			typedef std::map<uint64_t, std::map<uint64_t, double> >::iterator IteratorI;
			typedef std::map<uint64_t, double>::iterator IteratorJ;

			std::stringstream vs;
		  for (IteratorI i = table.begin(); i != table.end(); i++) {
				vs << "V" << i->first << ",";
			}

			if (!table.empty())
				CkPrintf("Comm[%d] --> localModularity %f (sigma_in = %f, sigma_tot = %f, m = %f), vertices: %s\n", 
						thisIndex, localModularity, sigma_in, sigma_tot, m, vs.str().c_str() );

			/*
		  for (IteratorI i = table.begin(); i != table.end(); i++) {
				std::stringstream s;
				for (IteratorJ j = i->second.begin(); j != i->second.end(); j++)
					s << "(" << j->first << "," << j->second << ") ";
				CkPrintf("Comm[%d] --> Vertex[%lld]: %s\n", thisIndex, i->first, s.str().c_str());
			}*/
		}
};

/***
 * CD Edge 
 ***/
struct CDEdge 
{
	uint64_t vid;	// destination vertex id
	double weight;	// edge weight
	bool local; // vertex is in the same supervertex

	uint64_t communityId;


	CDEdge(uint64_t vid, double weight) :
		vid (vid), weight (weight), communityId(vid), local(false) {}
};
bool operator< (const CDEdge& lhs, const CDEdge& rhs) { return lhs.weight < rhs.weight; }

/***
 * CD Vertex class. 
 ***/
class CDVertex : public CBase_CDVertex {
		private:
/*readonly*/ CProxy_CDVertex g;
/*readonly*/ CProxy_CDComm c;

				std::vector<CDEdge> edges;		// vertex edges
				uint64_t communityId;					// current supervertex id
				bool communityId_updated;

				double local_modularity;
				double internal_sum;  // sum of internal edges weights
				double external_sum; // sum of external edges weights

				double minWeight;
				uint64_t minVertexId;	
				std::vector<CDEdge>::iterator e;

				double totalWeight, localModularity;

				bool root;
				std::map<uint64_t, double> root_edges;		// root vertex edges

				int root_rcounter;

				double max_delta;
				uint64_t max_source;

				bool updated;

		public:
				CDVertex() : internal_sum(0), external_sum(0), max_delta(0), root(true), updated(true), root_rcounter(0) {
						// initialize vertex data
						communityId = thisIndex;
						communityId_updated = false;
				}
				void init(CProxy_CDVertex &g, CProxy_CDComm &c) { 
						this->g = g; 
						this->c = c;
				}

				/*entry*/ void acceptVertexData (VertexData &vdata) {
						double sum = 0;
						for (uint16_t i = 0; i < vdata.edge_dsts.size(); i++) {
								edges.push_back(CDEdge(vdata.edge_dsts[i], vdata.edge_weigths[i]));
								root_edges.insert(std::make_pair(vdata.edge_dsts[i], vdata.edge_weigths[i]));

								sum += vdata.edge_weigths[i];
								// graphio Random generator generates digraph, so we must
								// add respective edges to the destination node to make
								// graph undirected	
								//g[vdata.edge_dsts[i]].addEdge(thisIndex, vdata.edge_weigths[i]); 
						}

						//FIXME: do I really need sort?
						std::sort(edges.begin(), edges.end());

						contribute(sizeof(CmiFloat8), &sum, CkReduction::sum_double, 
	 							CkCallback(CkReductionTarget(Main, calcTotalEdgeWeight), mainProxy));
				}
				/*entry*/ void addEdge(uint64_t v, double w) 
				{
						edges.push_back(CDEdge(v,w));
						root_edges.insert(std::make_pair(v, w));

						//FIXME: do I really need sort?
						std::sort(edges.begin(), edges.end());
				}
				void print() 
				{
						std::stringstream s;
						for (std::vector<CDEdge>::iterator i = edges.begin(); i != edges.end(); i++) {
								s << i->vid << "(" << i->weight << "," << i->local << ") ";
						}

						if (root) 
							CkPrintf("CDVertex* %d (%lld):", thisIndex, communityId);
						else
							CkPrintf("CDVertex %d (%lld):", thisIndex, communityId);
					 
					  CkPrintf("iSum = %f, eSum = %f, Total weight = %f, localModularity = %f\n", 
								internal_sum, external_sum, totalWeight, localModularity);
						//CkPrintf("CDVertex %d (%lld), iSum = %f, eSum = %f, Edge list (sorted): %s\n", thisIndex, 
						//		communityId, internal_sum, external_sum, s.str().c_str());
						//CkPrintf("CDVertex %d (%lld), Edge list (sorted): %s, Min: %lld/%f\n", thisIndex, communityId, 
						//		s.str().c_str(), minVertexId, minWeight);
				}
				CDVertex(CkMigrateMessage *m) {}

				/*entry*/void doLouvainIteration_phase1() {
					//CkPrintf("CDVertex* %d (%lld): doLouvainIteration_phase1 called\n", thisIndex, communityId);

					if (updated) {
						std::map<uint64_t, double> weightsOfCommuinityConnections; 
						// calculate vertex connections to communities
						typedef std::vector<CDEdge>::iterator Iterator; 
						for (Iterator it = edges.begin(); it != edges.end(); it++) {
							//if (it->vid != thisIndex)
								weightsOfCommuinityConnections[it->communityId] += it->weight;
						}
						c[communityId].updateVertexCommunityConnections(thisIndex, weightsOfCommuinityConnections);
						updated = false;
					}
				}

				void updateCommunity(uint64_t comm) {
					//CkPrintf("CDVertex %d (%lld): updateCommunity called\n", thisIndex, communityId);
					communityId = comm;

					// broadcast new comm identifire to all neighbors
					typedef std::vector<CDEdge>::iterator Iterator; 
					for (Iterator it = edges.begin(); it != edges.end(); it++) {
						g[it->vid].updateNeighComm(thisIndex, communityId);
					}
					updated = true;
				}

				void updateNeighComm(uint64_t sv, uint64_t comm) {
					//CkPrintf("CDVertex %d (%lld): updateNeighComm called\n", thisIndex, communityId);
					typedef std::vector<CDEdge>::iterator Iterator; 
					for (Iterator it = edges.begin(); it != edges.end(); it++) {
						if (it->vid == sv) it->communityId = comm;
					}
					updated = true;
				}

				/*entry*/void recvNeighTotalWeight(uint64_t src, CmiFloat8 weight) {
					CkAssert(root);
					root_rcounter++;

					//calculate delta
					double delta = deltaFunction(root_edges.find(src)->second, totalWeight, 
							(internal_sum + external_sum), weight);

					if (thisIndex == 0)
						CkPrintf("CDVertex %d: recvNeighTotalWeight is called, <%lld, %f>, delta = %f\n", 
								thisIndex, src, weight, delta);

					// update delta and source
					if (delta > max_delta) {
						max_delta = delta;
						max_source = src;
					}

				}

				void updateEdgeList(uint64_t comm, std::map<uint64_t, double> &edgelist) {

				//	CkPrintf("CDVertex %d (%lld): updateEdgeList called\n", thisIndex, communityId);

				/*	if (comm != communityId)
						CkAbort("updateEdgeList error");*/

					edges.clear();
					typedef std::map<uint64_t, double>::iterator Iterator;
					for (Iterator it = edgelist.begin(); it != edgelist.end(); it++) {
						edges.push_back(CDEdge(it->first,it->second));
					}

					// set new communityId
					communityId = comm;

					// only 'updated' vertices will be part of the recontructed graph
					updated = true;
				}

				void updateCommunityIds() {
					if (!updated /*&& !root*/)
						g[communityId].getCommunityId(thisIndex);
				}

				void getCommunityId (uint64_t s) {
					g[s].setCommunityId(communityId);
				}

				void setCommunityId(uint64_t comm) {
					communityId = comm;
				}
};

/*mainchare*/
class Main : public CBase_Main {
	private:
/*readonly*/ CProxy_CDVertex g;
/*readonly*/ CProxy_CDComm c;

		int stepN, iterN;
		bool stopFlag;
		double modularity, modularity_old; // current and old modularities

		std::string ifile;
		std::string ofile;

		bool verify, verbose;

		uint64_t nVertex;
		uint64_t nEdges;

		double totalWeight;

#if defined(GRAPHHPC_CONTEST)
		typedef struct {
			double time; 
			double modularity;
		} perf_t;

		struct timespec start_ts, finish_ts;
		struct timespec start_ts_build, finish_ts_build;
		double build_time;
		perf_t perf, avg_perf;
		double modularity_reference;
#endif

	public:
		Main(CkArgMsg* m) : iterN(0), stepN(0), modularity(-1), totalWeight(0), modularity_old(-1)
		{
#if defined(GRAPHHPC_CONTEST)
			modularity_reference = -1;
#endif

			parseCommandOptions(m->argc, m->argv);

			CkPrintf("Running Community Detection (Louvain) on %d processors for graph of scale %d \n",
					CkNumPes(), nScale);
			mainProxy = thisProxy;

			// create io chares
			//io = CProxy_IO<VertexData, CProxy_CDVertex, GraphFileIOFormat::GraphHPC /*FIXME*/, 
			//   GeneratorType::Random, DistrMode::Block>::ckNew(nScale, nK); 

			if (!ifile.empty())
				io = CProxy_IO<VertexData, CProxy_CDVertex, GraphFileIOFormat::GraphHPC, 
					 GeneratorType::Random, DistrMode::Block>::ckNew(ifile); 
			else {
				CkError("input file is not set");
				CkExit();
			}

			// request graph properties (size)
			CkCallback cb(CkIndex_Main::recvGraphProps(NULL), mainProxy);
			io.ckLocalBranch()->reqGraphProps(cb);

			// load graph after initialization is complete
			CkStartQD(CkIndex_Main::init(), &thishandle);
		};

		/*enbrty*/ void recvGraphProps(GraphPropsMsg *m) {
			CkPrintf("recvGraphProps called: %lld\n", m->nVertex);
			nVertex = m->nVertex;
			nEdges= m->nEdges;
		}

		/*entry*/void init() {
			CkPrintf("Main::init  called\n");

			// create chare array
			g = CProxy_CDVertex::ckNew(nVertex /*1<<nScale*/);

			// create chare array
			c = CProxy_CDComm::ckNew(nVertex /*1<<nScale*/);

			// make all necessare initilizations
			g.init(g,c);
			c.init(g,c);
			io.init(g);

			// load graph after initialization is complete
			CkStartQD(CkIndex_Main::loadGraph(), &thishandle);
		}

		/*entry*/void loadGraph() {
			// load graph to chare array
			io.loadGraphToChares();

#if defined(GRAPHHPC_CONTEST)
			clock_gettime(CLOCK, &start_ts_build);
#endif
		}

		/*entry*/ void calcTotalEdgeWeight(CmiFloat8 tot) {
			CkPrintf("Total weight %f\n", tot);
			c.init(tot);
			//CkStartQD(CkIndex_Main::exit(), &thishandle);
			CkStartQD(CkIndex_Main::doLouvainIteration(), &thishandle);

#if defined(GRAPHHPC_CONTEST)
			clock_gettime(CLOCK, &finish_ts_build);
			build_time = (finish_ts_build.tv_nsec - (double)start_ts_build.tv_nsec) * 1.0e-9 + (finish_ts_build.tv_sec - (double)start_ts_build.tv_sec);
			clock_gettime(CLOCK, &start_ts);
#endif
		}

		void doLouvainIteration() {
			
			CkPrintf("--------------------------------------------------------------------------------------\n");
			CkPrintf("Do Louvain iteration: iteration = %d \n", iterN);
			CkPrintf("Refine phase:\n");
			mainProxy.doLouvainIteration_phase1(); 
			iterN++;
		}

		/*entry*/ void doLouvainIteration_phase1() {
			g.doLouvainIteration_phase1();
			CkStartQD(CkIndex_Main::doLouvainIteration_phase1_reportModularity(), &thishandle);
			stepN++;
		}

		/*entry*/ void doLouvainIteration_phase1_reportModularity () {
			c.calculateModularity();
		} 

		void reportModularity(CmiFloat8 mod) {
			double eps = 0.001;

			double delta = mod - modularity;
			CkPrintf("----->iter = %d, modularity = %f (+%e,%.2f\%), %e\n", 
					stepN, mod, delta, (delta/std::abs(modularity))*100, std::abs(modularity) * eps);

			//if (delta < 0)
			//	CkAbort("Negative modularity increment");

			if (delta < std::abs(modularity) * eps) {
				modularity = mod;

				if (stepN == 2/*FIXME: fix iteration counter*/)	{
					// if no changes in graph after reconstruction improved its
					// modularity then we are done :-)
					CkPrintf("Done.\n");
					mainProxy.done();
				} else {
					// reconstruct graph (make new community graph)
					mainProxy.doLouvainIteration_phase2(); 
					//c.getTotalWeight();
				} 

			} else {
				modularity = mod;
				//CkExit();
				doLouvainIteration_phase1_1();
			}

		}

		/*entry*/ void doLouvainIteration_phase1_1() {
			//if (stepN > 10) CkExit();

			//CkPrintf("Phase 1.1:\n");
			c.searchMaxDelta();
			CkStartQD(CkIndex_Main::doLouvainIteration_phase1_2(), &thishandle);
		}

		/*entry*/ void doLouvainIteration_phase1_2() {
			//CkPrintf("Phase 1.2:\n");
			c.filterTransitions();
			CkStartQD(CkIndex_Main::doLouvainIteration_phase1_3(), &thishandle);
		}

		/*entry*/ void doLouvainIteration_phase1_3() {
			//CkPrintf("Phase 1.2:\n");
			c.doTransitions();
			CkStartQD(CkIndex_Main::doLouvainIteration_phase1(), &thishandle);
		}

		/*entry*/ void doLouvainIteration_phase2() {
			CkPrintf("--------------------------------------------------------------------------------------\n");
			CkPrintf("Phase 2: Reconstruct community graph\n");
			c.reconstructCommunityGraph();
			//g.doLouvainIteration_phase2();
			CkStartQD(CkIndex_Main::doLouvainIteration_phase2_1(), &thishandle);
			//CkExit();
		}
		/*entry*/ void doLouvainIteration_phase2_1() {
			CkPrintf("Phase 2_1:\n");
			g.updateCommunityIds();
			//g.doLouvainIteration_phase2();
			stepN = 0;
			modularity = -1;
			CkStartQD(CkIndex_Main::doLouvainIteration(), &thishandle);
			//CkStartQD(CkIndex_Main::exit(), &thishandle);
			//CkExit();
		}

		void getTotalWeight(double &w) {
			totalWeight = w;
			CkPrintf("Total weight = %f\n", w);
			CkExit();
		}

		/*entry*/void setStopFlag(bool flag) {
			stopFlag = flag;
		}

		void done() 
		{
			CkPrintf("Louvain finished\n");
			//g.print();

#if defined(GRAPHHPC_CONTEST)
			clock_gettime(CLOCK, &finish_ts);
			perf.time = (finish_ts.tv_nsec - (double)start_ts.tv_nsec) * 1.0e-9 + (finish_ts.tv_sec - (double)start_ts.tv_sec);
			perf.modularity = modularity/*modularity(graph, component_id)*/;
			std::cout << "finished. Time is " << perf.time << " secs. Modularity is " << perf.modularity << endl;
			avg_perf.time = perf.time;
			avg_perf.modularity  = perf.modularity;
			std::cout.precision(2);
			std::cout << ifile << ":";
			std::cout << "    nIters: " << 1 /*nIters*/;
			std::cout << "    build_time: " << fixed << build_time << " seconds";
			std::cout << "    avg_time: " << fixed << avg_perf.time << " seconds";
			std::cout.precision(5);
			std::cout << "    modularity: " << avg_perf.modularity;

			modularity_reference = (modularity_reference > 0) ? modularity_reference : avg_perf.modularity;
			std::cout << "    mod_ref: " << modularity_reference;

			double mod_diff = avg_perf.modularity - modularity_reference;
			double rating_coef = mod_diff > 0 ? (avg_perf.modularity / modularity_reference - 1)*10 : avg_perf.modularity / modularity_reference;
			double rating = 10000 * rating_coef / avg_perf.time;
			std::cout << "    RATING: " << fixed << rating << endl;
#endif
			CkExit();
		}

		void exit() 
		{
			CkExit();
		}

		void usage(int argc, char **argv)
		{
			printf("Usage:\n");
			printf("    %s -in <input> [options] \n", argv[0]);
			printf("Options:\n");
			printf("    -in <input> -- input graph filename\n");
			printf("    -nIters <nIters> -- number of iterations. By default 4\n");
			printf("    -mod_ref -- reference modularity for rating function\n");
			CkExit();
		}
		void parseCommandOptions(int argc, char **argv)
		{
			if (argc == 1) usage(argc, argv);

			for (int i = 1; i < argc; ++i) {
				if (!strcmp(argv[i], "-in")) {
					ifile = argv[++i];
					//int l = strlen(argv[++i]);
					//strncpy(filename, argv[i], (l > FNAME_LEN - 1 ? FNAME_LEN - 1 : l) );
				}
				if (!strcmp(argv[i], "-nIters")) {
					//nIters = (int) atoi(argv[++i]);
				}
				if (!strcmp(argv[i], "-mod_ref")) {
#if defined(GRAPHHPC_CONTEST)
					modularity_reference = atof(argv[++i]);
#endif
				}
			}
		}

		/*
		void parseCommandOptions(int argc, char **argv) {
			while (1) {
				int option_index = 0;
				static struct option long_options[] = {
					{"in", 		1, 0, 'i'},
					{"out", 		1, 0, 'o'},
					{"verify", 		0, 0, 'v'},
					{"verbose", 		0, 0, 'e'},
					{"generate", 		0, 0, 'g'},
					{"mod_ref", 		1, 0, 'x'},
					{"help", 		0, 0, 'h'},
					{0,0,0,0}
				};

				char c = getopt_long(argc, argv, "i:o:x:veg",
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
#if defined(GRAPHHPC_CONTEST)
					case 'x':
						modularity_reference = atof(optarg);
						break;
#endif
					case 'v':
						verify = true;
						break;
					case 'e':
						verbose = true;
						break;
					case 'h':
						CkPrintf("Usage:\n");
						CkPrintf("   cd_louvain_naive [OPTIONS]\n\n");
						CkPrintf("Options:\n");
						CkPrintf("   --in <INFILE> 			         \tspecifies file with input graph                                     \n");
						CkPrintf("   --out <INFILE>			         \tspecifies file for output                                           \n");
					default:
						break;
				}

			}
		}
		*/

		void printCommandOptions() {

			CkPrintf("Command options:\n");
			CkPrintf("\tifile = %s\n", ifile.c_str());
			CkPrintf("\tofile = %s\n", ofile.c_str());
		}
};

/*class GraphGen : public CBase_GraphGen {
	public:
		GraphGen(CProxy_BFSVertex &g, int scale) {}
};*/


#include "cd_louvain_naive.def.h"
