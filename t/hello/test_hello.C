#include <uChareLib.h>
#include <test_hello.decl.h>

CProxy_Main mainProxy;

// Number of uchares per PE
int numElementsPerPe;

// Number of chares per PE
int numChareSetsPerPe;

class Hello : public CBase_Hello, public uChare<Hello, CProxy_Hello, CBase_Hello> {
	public:
		Hello() {}
		Hello(std::size_t index, uChareSet<Hello, CProxy_Hello, CBase_Hello> *uchareset) : 
			uChare<Hello, CProxy_Hello, CBase_Hello>(index, uchareset)  {
			contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
		}

		void hello(int callee) { 
			CkPrintf("[chare=%d,pe=%d]: %d: hello called by %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
			getProxy()[callee]->buybuy(getId());
		}

		void buybuy(int callee) { 
			CkPrintf("[chare=%d,pe=%d]: %d: buybuy called by %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
		}

		void run() { 
			CkAbort("Hello: run() called");
		} 
		void run(int x) { 
			//CkPrintf("%d:Hello: run(int) called\n", getId());
			if ((getId() + 1) < N_uChares)
				getProxy()[getId()+1]->hello(getId());
			else
				getProxy()[0]->hello(getId());
		} 
};

class Main : public CBase_Main {
	private:
		CProxy_uChareSet<Hello, CProxy_Hello, CBase_Hello> uchareset_proxy;
	public:
		Main(CkArgMsg *args) {
			CkPrintf("%d:Hallo!\n", CmiMyPe());
			numElementsPerPe = atoi(args->argv[1]);
			numChareSetsPerPe = atoi(args->argv[2]);
			N_uChareSets = CkNumPes() * numChareSetsPerPe;
			N_uChares = CkNumPes() * numElementsPerPe;
			mainProxy = thisProxy;

			CkCallback cb(CkIndex_Main::start(), thisProxy);
		  uchareset_proxy = CProxy_uChareSet<Hello, CProxy_Hello, CBase_Hello>::ckNew(cb, N_uChareSets);
			/*for (int i = 0; i < N_uChares; i++) {
				Hello hello_uchare(i);
				CkPrintf("Main: add uchare object %d to uChareSet %d\n", i, uChareDistribution::getuChareSetId(i));
				uchareset_proxy[uChareDistribution::getuChareSetId(i)].add(hello_uchare);
			}*/
			//CkExit();
		}

		void start() {
				CkPrintf("Main: initialization completed (distributed graph created)\n");
				CkPrintf("Main: run calculations...\n");
				CkCallback endCb(CkIndex_Main::done(), thisProxy);
				uchareset_proxy.run(0, endCb);
		}

		void done() {
			CkPrintf("Main: done\n");
			CkExit();
		}
};

#include "test_hello.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChareSet.def.h"
#undef CK_TEMPLATES_ONLY
