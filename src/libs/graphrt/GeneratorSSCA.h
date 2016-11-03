#ifndef __GeneratorSSCA_h__
#define __GeneratorSSCA_h__

namespace GraphLib {

	template <class Graph, class Opts, VertexMapping vertexMapping, GraphType graphType>
		class SSCA_Generator_Charm;

	template <class Graph, class Opts, VertexMapping vertexMapping, GraphType graphType, class CProxy_Aggregator>
		class SSCA_Generator_Tram;

	template <class Graph, class Opts>
		class SSCA_Generator;

	template <class Graph, class Opts, GraphType graphType>
		class SSCA_Generator_Charm<Graph, Opts, VertexMapping::SingleVertex, graphType> : 
			public CBase_SSCA_Generator_Charm <Graph, Opts, VertexMapping::SingleVertex, graphType>, 
				public SSCA_Generator<Graph, Opts> {
			public:
				SSCA_Generator_Charm(Graph & g, Opts & opts) : g(g),  SSCA_Generator<Graph, Opts>(g, opts) {}

				void generate(CkCallback & cb) {
					if (CmiMyPe() == 0) {
						CkPrintf("SSCA generation started\n");
						this->info();
						// do clique generation
						CkPrintf("\tgenerating cliques...");
						this->thisProxy.do_cliqueGeneration();
						CkPrintf("...done\n");
						CkStartQD(CkCallbackResumeThread());
						// do edge generation
						CkPrintf("\tgenerating edges...");
						this->thisProxy.do_edgeGeneration();
						CkPrintf("...done\n");
						CkStartQD(CkCallbackResumeThread());
						// return control to application 
						cb.send();
					}
				}

				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					g.getProxy()[e.first].connectVertex(typename Graph::Edge(e.second, w));
					g.getProxy()[e.second].connectVertex(typename Graph::Edge(e.first, w));
				}

				void sndMsg_addVertexToClique(int pe, CmiUInt8 v, CmiUInt8 c) {
					this->thisProxy[pe].addVertexToClique(v,c);
				}

				void sndMsg_addInterCliqueEdge(int pe, CmiUInt8 v, CmiUInt8 u, CmiUInt8 c) {
					this->thisProxy[pe].addInterCliqueEdge(v,u,c);
				}

			private:
				Graph g;
		};

	template <class Graph, class Opts, GraphType graphType, class CProxy_Aggregator>
		class SSCA_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, CProxy_Aggregator> : 
			public CBase_SSCA_Generator_Tram <Graph, Opts, VertexMapping::SingleVertex, graphType, CProxy_Aggregator>, 
				public SSCA_Generator<Graph, Opts> {
			public:
				SSCA_Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : 
					g(g), aggregator(aggregator), SSCA_Generator<Graph, Opts>(g, opts) {}

				void generate(CkCallback & cb) {
					if (CmiMyPe() == 0) {
						CkPrintf("SSCA generation started\n");
						this->info();
						// do clique generation
						CkPrintf("\tgenerating cliques...");
						this->thisProxy.do_cliqueGeneration();
						CkPrintf("...done\n");
						CkStartQD(CkCallbackResumeThread());
						// do edge generation
						CkPrintf("\tgenerating edges...");
						this->thisProxy.do_edgeGeneration();
						CkPrintf("...done\n");
						CkStartQD(CkCallbackResumeThread());
						//this->thisProxy.dump_cliques();
						//CkStartQD(CkCallbackResumeThread());
						// return control to application 
						cb.send();
					}
				}

				void addEdge(const std::pair<uint64_t,uint64_t> & e) {
					double w = drand48();
					ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter>
						* localAggregator = aggregator.ckLocalBranch();
					localAggregator->insertData(Edge(e.second, w), e.first);
				}

				void sndMsg_addVertexToClique(int pe, CmiUInt8 v, CmiUInt8 c) {
					this->thisProxy[pe].addVertexToClique(v,c);
				}

				void sndMsg_addInterCliqueEdge(int pe, CmiUInt8 v, CmiUInt8 u, CmiUInt8 c) {
					this->thisProxy[pe].addInterCliqueEdge(v,u,c);
				}

			private:
				Graph g;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Aggregator aggregator;
		};

	template <class Graph, class Opts>
		class SSCA_Generator {
			public:
				SSCA_Generator(Graph & g, Opts & opts) : g(g), opts(opts), graphProxy(g.getProxy()) {
					numParallelEdges = opts.ssca.numParallelEdges; 
					foreward = opts.ssca.foreward;
					backward = opts.ssca.backward;
					scale = opts.scale;
					totalVertex = 1 << scale;
					totalClique = totalVertex / opts.ssca.maxCliqueSize;
					vertexToClique = std::vector<CmiUInt8>(totalVertex / CmiNumPes(), -1);
					cliques = std::vector<Clique>(totalClique / CmiNumPes());
					srandom(CmiMyPe()); // FIXME: neet to use more smart approach to pseudo-random stream generation
				}
				virtual void addEdge(const std::pair<uint64_t,uint64_t> & e) = 0; 
				virtual void sndMsg_addVertexToClique(int pe, CmiUInt8 v, CmiUInt8 c) = 0;
				virtual void sndMsg_addInterCliqueEdge(int pe, CmiUInt8 v, CmiUInt8 u, CmiUInt8 c) = 0;

				void info() {
					CkPrintf("SSCA graph info: scale = %d, max clique = %lld\n", opts.scale, opts.ssca.maxCliqueSize);
				}

				void addVertexToClique(CmiUInt8 v, CmiUInt8 c) {
					CmiUInt8 startCliqueIndex = CmiMyPe() * totalClique / CmiNumPes();
					CkAssert (c >= startCliqueIndex && 
							c < (startCliqueIndex + totalClique / CmiNumPes()));
					cliques[c % (totalClique / CmiNumPes())].push_back(v);
				}

				void addInterCliqueEdge(CmiUInt8 v, CmiUInt8 u, CmiUInt8 cv) {
					CmiUInt8 startVertexIndex = CmiMyPe() * totalVertex / CmiNumPes();
					CkAssert (u >= startVertexIndex && 
							u < (startVertexIndex + totalVertex / CmiNumPes()));
					if (vertexToClique[u % (totalVertex / CmiNumPes())] != cv)
						// the u vertex belongs to local pe
						for(int k = 0; k < numParallelEdges; k++) {
							if (foreward)
								addEdge(std::make_pair(v, u));
							if (backward)
								addEdge(std::make_pair(u, v));
						}
				}

				void do_cliqueGeneration() {
					CmiUInt8 startVertexIndex = CmiMyPe() * totalVertex / CmiNumPes();
					CmiUInt8 startCliqueIndex = CmiMyPe() * totalClique / CmiNumPes();
					// distribute vertices between cliques
					for(CmiUInt8 i = 0; i < totalVertex / CmiNumPes(); i++) {
						CmiUInt8 v = startVertexIndex + i;
						CmiUInt8 c = random() % totalClique;
						//CkPrintf("%d: %lld --> %lld\n", CmiMyPe(), v, c);
						vertexToClique[v % (totalVertex / CmiNumPes())] = c;
						if (c >= startCliqueIndex && 
								c < (startCliqueIndex + totalClique / CmiNumPes()))
							// the vertex belongs to local clique
							cliques[c % (totalClique / CmiNumPes())].push_back(v);
						else 
							// the vertex belongs to remote clique
							//this->thisProxy[c / (totalClique / CmiNumPes())].addVertexToClique(v,c);
							sndMsg_addVertexToClique (c / (totalClique / CmiNumPes()), v, c);
					}
				}

				void do_edgeGeneration() {
					CmiUInt8 startVertexIndex = CmiMyPe() * totalVertex / CmiNumPes();
					CmiUInt8 startCliqueIndex = CmiMyPe() * totalClique / CmiNumPes();
					// generate intra-clique edges
					for(CmiUInt8 c = 0; c < totalClique / CmiNumPes(); c++) {
						Clique & clique = cliques[c];
						for(CmiUInt8 i = 0; i < clique.size(); i++)
							for(CmiUInt8 j = i + 1; j < clique.size(); j++)
								for(int k = 0; k < numParallelEdges; k++) {
									if (foreward)
										addEdge(std::make_pair(clique[i], clique[j]));
									if (backward)
										addEdge(std::make_pair(clique[j], clique[i]));
								}
					}
					// generate inter-clique edges
					for(CmiUInt8 i = 0; i < totalClique / CmiNumPes(); i++) {
						CmiUInt8 v = startVertexIndex + i;
						for(CmiUInt8 j = 2; j < totalVertex; j *= 2) {
							CmiUInt8 u = (v + j) % totalVertex;
							if (u >= startVertexIndex && 
									u < (startVertexIndex + totalVertex / CmiNumPes()))
								if (vertexToClique[v % (totalVertex / CmiNumPes())] !=
										vertexToClique[u % (totalVertex / CmiNumPes())])
									// the u vertex belongs to local pe
									for(int k = 0; k < numParallelEdges; k++) {
										if (foreward)
											addEdge(std::make_pair(v, u));
										if (backward)
											addEdge(std::make_pair(u, v));
									}
								else
									// the u vertex belongs to remote pe
									//this->thisProxy[u / (totalVertex / CmiNumPes())].addInterCliqueEdge(v, u, 
									//		vertexToClique[v % (totalVertex / CmiNumPes())]);
									sndMsg_addInterCliqueEdge(u / (totalVertex / CmiNumPes()), v, u, 
											vertexToClique[v % (totalVertex / CmiNumPes())]);
						}
					}
				}

				void dump_vertices() {
					CmiUInt8 startVertexIndex = CmiMyPe() * totalVertex / CmiNumPes();
					CkPrintf("%d: ", CmiMyPe());
					for(CmiUInt8 i = 0; i < totalVertex / CmiNumPes(); i++) {
						CmiUInt8 v = startVertexIndex + i;
						CkPrintf("%lld(%lld)", v, vertexToClique[v % (totalVertex / CmiNumPes())]);
					}
					CkPrintf("\n");
				}

				void dump_cliques() {
					CmiUInt8 startCliqueIndex = CmiMyPe() * totalClique / CmiNumPes();
					for(CmiUInt8 i = 0; i < totalClique / CmiNumPes(); i++) {
						CmiUInt8 c = startCliqueIndex + i;
						CkPrintf("%lld: ", c);
						Clique & clique = cliques[c % (totalClique / CmiNumPes())];
						for(CmiUInt8 j = 0; j < clique.size(); j++)
							CkPrintf("%lld, ", clique[j]);
						CkPrintf("\n");
					}
				}

				void dump_edges() {
					typedef std::vector<std::pair<CmiUInt8,CmiUInt8>>::iterator Iterator;
					for(Iterator i = edges.begin(); i != edges.end(); i++)
						CkPrintf("(%lld, %lld)\n", i->first, i->second);
				}

			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				int scale;
				CmiUInt8 totalVertex;
				CmiUInt8 totalClique;
				typedef std::vector<CmiUInt8> Clique;
				std::vector<Clique> cliques;
				std::vector<CmiUInt8> vertexToClique;
				int numParallelEdges;
				bool foreward, backward;
				std::vector<std::pair<CmiUInt8,CmiUInt8>> edges;
				Opts opts;
		};
}



#endif

