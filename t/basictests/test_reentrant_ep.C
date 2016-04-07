#include <uChareLib.h>
#include <test_reentrant_ep.decl.h>

CmiUInt8 N;
CProxy_Main mainProxy;

class Hello : public CBase_uChare_Hello {
	private:
		int counter, zero;
	public:
		Hello(const uChareAttr_Hello &attr) : counter(0), zero(0), CBase_uChare_Hello(attr) {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			contribute(CkCallback(CkReductionTarget(Main, init), mainProxy));
		}

		void start__reentrant() {
			//CmiUInt8 peer = (thisIndex + N / 2) % N;
			CmiUInt8 peer = thisIndex % 2 ? (thisIndex - 1) : (thisIndex + 1);
			thisProxy[peer]->ping__reentrant(thisIndex);
		}

		void start__nonreentrant() {
			//CmiUInt8 peer = (thisIndex + N / 2) % N;
			CmiUInt8 peer = thisIndex % 2 ? (thisIndex - 1) : (thisIndex + 1);
			thisProxy[peer]->ping__nonreentrant(thisIndex);
		}

		void ping__nonreentrant(int callee) { 
			if (zero) CkAbort("zero is non-null");

			zero++;
			if (counter++ < 10) {
				thisProxy[callee]->ping__nonreentrant(thisIndex);
			}
			zero--;
		}

		void ping__reentrant(int callee) { 
			//if (zero) CkPrintf("ping__reentrant:: nested call detected\n");
			zero++;
			if (counter++ < 10) {
				thisProxy[callee]->ping__reentrant(thisIndex);
			}
			zero--;
		}

		void print() {
			CkPrintf("[chare=%lld,pe=%d]: %lld: counter = %d, zero = %d\n", 
					thisIndex, CmiMyPe(), thisIndex, counter, zero);
		}

		void clear() {
			zero = 0; counter = 0;
		}
};
/**********************************************************************************************/

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		CProxy_uChare_Hello hello;

		int nElements;
		int nChares;

	public:
		Main(CkArgMsg *m) {
			if (m->argc > 1) {
				N = CkNumPes() * (1 << atoi(m->argv[1]));
			} else {
				// default values for nElements and nChares
				N = CkNumPes() * (1 << 4);
			}

			CkPrintf("Hello test started:\n");
			CkPrintf("uchares = %lld, chare sets per PE = %d\n", N, 1);
			mainProxy = thisProxy;
			hello = CProxy_uChare_Hello::ckNew(N, CkNumPes());

			delete m;
		}

		void init() {
			hello.run(CkCallback(CkIndex_Main::start__reentrant(), thisProxy));
		}

		void start__reentrant() {
			CkPrintf("Main: start [reentrant]...\n");
			startt = CkWallTimer();
			hello.start__reentrant();
			CkStartQD(CkIndex_Main::done__reentrant(), &thishandle);
		}
		void done__reentrant() {
			endt = CkWallTimer();
			CkPrintf("Main: done [reentrant]\n");
			//hello.print();
			hello.clear();
			CkStartQD(CkIndex_Main::start__nonreentrant(), &thishandle);
		}

		void start__nonreentrant() {
			CkPrintf("Main: start [nonreentrant]...\n");
			startt = CkWallTimer();
			hello.start__nonreentrant();
			CkStartQD(CkIndex_Main::done__nonreentrant(), &thishandle);
		}
		void done__nonreentrant() {
			endt = CkWallTimer();
			CkPrintf("Main: done [nonreentrant]\n");
			//hello.print();
			hello.clear();
			CkStartQD(CkIndex_Main::done(), &thishandle);
		}

		void done() {
			endt = CkWallTimer();
			CkPrintf("Main: done\n");
			//CkStartQD(CkIndex_Main::exit(), &thishandle);
			CkExit();
		}

		void exit() {
			CkExit();
		}
};

#include "test_reentrant_ep.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
