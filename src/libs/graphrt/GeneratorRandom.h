#ifndef __GeneratorRandom_h__
#define __GeneratorRandom_h__

#include "Generator.h"

namespace GraphLib {

	template <class Graph, class Opts>
		class Random_Generator;

	template <class Graph, class Opts, VertexMapping vertexMapping>
		class Random_Generator_Charm :
			public CBase_Random_Generator_Charm <Graph, Opts, vertexMapping>,
		 	public Base_Generator_Charm<Graph, Opts, vertexMapping>,
				Random_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				CProxy_Graph graphProxy;
				Graph g;
				public:
					Random_Generator_Charm(Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()), 
						Random_Generator<Graph, Opts>(g, opts), 
						Base_Generator_Charm<Graph,Opts,vertexMapping>(g) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						Random_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(const std::pair<uint64_t,uint64_t> & e) {
						Base_Generator_Charm<Graph,Opts,vertexMapping>::addEdge(e);
					}
			};

	template <class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator>
		class Random_Generator_Tram :
			public CBase_Random_Generator_Tram <Graph, Opts, vertexMapping, CProxy_Aggregator>,
			public Base_Generator_Tram<Graph, Opts, vertexMapping, CProxy_Aggregator>,
				Random_Generator<Graph, Opts> {
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				CProxy_Aggregator aggregator;
				public:
					Random_Generator_Tram(Graph & g, Opts & opts, CProxy_Aggregator & aggregator) : g(g), aggregator(aggregator), 
						Base_Generator_Tram<Graph, Opts, vertexMapping, CProxy_Aggregator>(g, aggregator),
						Random_Generator<Graph, Opts>(g, opts) {}
					void generate(CkCallback & cb) {
						if (CmiMyPe() == 0) {
							// do edge generation
							this->thisProxy.do_edgeGeneration();
							CkStartQD(CkCallbackResumeThread());
							// return control to application 
							cb.send();
						}
					}
					void do_edgeGeneration() { 
						Random_Generator<Graph, Opts>::do_generate(); 
					}
					void addEdge(const std::pair<uint64_t,uint64_t> & e) {
						Base_Generator_Tram<Graph,Opts,vertexMapping,CProxy_Aggregator>::addEdge(e);
					}
			};


	template <class Graph, class Opts>
		class Random_Generator {
			public:
				Random_Generator (Graph & g, Opts & opts) : g(g), opts(opts) {
					scale = opts.scale;
					totalVertex = 1 << scale;
					srandom(1); 
					totalEdge = totalVertex * opts.K;
					for(CmiUInt8 i = 0; i < CmiMyPe() * totalEdge / CmiNumPes() ; i++)
						random(); 
				}
				virtual void addEdge(const std::pair<uint64_t,uint64_t> & e) = 0;
				void do_generate() {
					for(CmiUInt8 i = 0; i < totalEdge / CmiNumPes() ; i++) {
						addEdge(std::make_pair(random(),random())); 
					}
				}

			private:
				typedef typename Graph::Proxy CProxy_Graph;
				typedef typename Graph::Vertex Vertex;
				typedef typename Graph::Edge Edge;
				Graph g;
				int scale;
				CmiUInt8 totalVertex;
				CmiUInt8 totalEdge;
				Opts opts;
		};
}



#endif
