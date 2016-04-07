#include <stdio.h>
#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GraphDistr.h"
#include "GraphFileIO.h"
#include "GraphGenerator.h"
#include "InputOutput.decl.h"

template <class C, GraphFileIOFormat D>
class Foo : public CBase_Foo<C, D> {
public:
	Foo() {
		CkPrintf("Foo[%d]: hello!\n", CmiMyPe());
	}
};

class GraphPropsMsg : public CMessage_GraphPropsMsg {
	public:
		uint64_t nVertex;
		uint64_t nEdges;
		GraphPropsMsg(uint64_t nVertex, uint64_t nEdges) : 
			nVertex(nVertex), nEdges(nEdges) {}
};

template <class VertexData, class CProxy_Chares, GraphFileIOFormat F = GraphFileIOFormat::GraphHPC, 
		 GeneratorType G = GeneratorType::Random, DistrMode D = DistrMode::Block >
class IO : public CBase_IO<VertexData, CProxy_Chares, F, G, D> {
private:
	GraphIO<VertexData> *io;
	GraphGenerator<VertexData> *generator;
	DistrMode mode;
	uint64_t nVertex;
	uint64_t nEdges;
	uint16_t myPE;
	uint16_t nPEs;

	std::vector<uint64_t> registeredChares;
	CProxy_Chares proxyChares;

	
public:
	IO(uint16_t nScale, uint16_t K) {
		nVertex = 1 << nScale;
		myPE = CmiMyPe();
		nPEs = CmiNumPes();
		CkPrintf("IO[%d]: Generator (%lld) created\n", myPE, nVertex);
		std::vector<char> *v = new std::vector<char>(256);
		if (G == GeneratorType::Random)
			io = new Random_GraphGenerator<VertexData>(nScale, K); 
	}
	IO(std::string &filename) {
		myPE = CmiMyPe();
		nPEs = CmiNumPes();
		if (F == GraphFileIOFormat::GraphHPC)
			io = new GraphHPC_GraphFileIO<VertexData> (filename);		
		nVertex = io->getVertexNum(); 
		nEdges = io->getEdgesNum(); 
	}
	~IO(){
	}
	/*entry*/ void init(CProxy_Chares proxyChares) {
		this->proxyChares = proxyChares;
	}
	/*entry*/ void registerChare(uint64_t chareId) { 
		registeredChares.push_back(chareId);
		std::sort(registeredChares.begin(), registeredChares.end());
	}
	/*entry*/ void loadGraphToChares() {
		CkPrintf("IO[%d]: start loading graph to chares...\n", CmiMyPe());
		VertexData vdata;
		for (uint64_t i = 0; i < nVertex; i++) {
			uint64_t vertexId = io->getVertex(vdata);
			if (myPE == toPEId(vertexId, nVertex, nPEs, D)) {
				proxyChares[toChareId(vertexId, nVertex, nPEs, D)]->acceptVertexData(vdata);
			}
		}
		CkPrintf("IO[%d]: ...end loading.\n", CmiMyPe());
	}

	inline uint64_t toChareIndex(uint64_t vertexIndex) {
		uint64_t chareIndex;
		if (D == DistrMode::Block) {
			chareIndex = vertexIndex;
		} else {
			CkError("Cyclic distribution is not supported");
		}
		return chareIndex;
	}

	void reqGraphProps(const CkCallback& cb) {
		cb.send(new GraphPropsMsg(nVertex, nEdges));
		//cb.send(new GraphPropsMsg(1000));
	}
};

#define CK_TEMPLATES_ONLY
#include "InputOutput.def.h"
#undef CK_TEMPLATES_ONLY
