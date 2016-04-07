#include <uChareLib.h>
#include <test_hello_tram.decl.h>

CmiUInt8 N;
CProxy_Main mainProxy;

class Hello : public CBase_uChare_Hello {
	public:
		Hello(const uChareAttr_Hello &attr) : CBase_uChare_Hello(attr) {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			contribute(CkCallback(CkReductionTarget(Main, init), mainProxy));
		}

		void ping(int callee) { 
			//CkPrintf("[chare=%d,pe=%d]: %d: ping\n", 
			//		getuChareSet()->getId(), getuChareSet()->getPe(), getId());
			contribute(CkCallback(CkReductionTarget(Main, done), mainProxy));
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
			hello.run(CkCallback(CkIndex_Main::start(), thisProxy));
		}

		/*entry*/ void start() {
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();
			hello.ping(100);
			hello.flush();
		}

		void done() {
			endt = CkWallTimer();
			CkPrintf("Main: done\n");
			CkExit();
		}
};

#include "test_hello_tram.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
