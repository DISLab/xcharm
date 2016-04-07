#include <uChareLib.h>
#include <test_contribute.decl.h>

CmiUInt8 N;
CProxy_Main mainProxy;

class Hello_charm_ref : public CBase_Hello_charm_ref {
	public:
		Hello_charm_ref() {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
		}
		Hello_charm_ref(CkMigrateMessage * m) {}

		void call_contribute(/*CkReduction::reducerType op,*/ const CmiUInt8 & v) { 
			contribute(sizeof(CmiUInt8), &v, CkReduction::sum_long,
					        CkCallback(CkReductionTarget(Main, verify_contribute), mainProxy));
		}
};

class Hello : public CBase_uChare_Hello {
	public:
		Hello(const uChareAttr_Hello &attr) : CBase_uChare_Hello(attr) {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			contribute(CkCallback(CkReductionTarget(Main, init), mainProxy));
		}

		void call_contribute(/*CkReduction::reducerType op,*/ const CmiUInt8 & v) { 
			contribute(sizeof(CmiUInt8), &v, CkReduction::sum_long,
					        CkCallback(CkReductionTarget(Main, verify_contribute), mainProxy));
		}
};
/**********************************************************************************************/

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		CProxy_uChare_Hello hello;
		CProxy_Hello_charm_ref hello_ref;

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
			//hello_ref = CProxy_Hello_charm_ref::ckNew(N);

			delete m;
		}

		void init() {
			hello.run(CkCallback(CkIndex_Main::start(), thisProxy));
		}

		/*entry*/ void start() {
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();
			hello.call_contribute(/*CkReduction::sum_long,*/ 1);
			//hello_ref.call_contribute(1);
		}

		void done() {
			endt = CkWallTimer();
			CkPrintf("Main: done\n");
			CkExit();
		}

		void verify_contribute(long r) {
			CkPrintf("N = %lld, r = %lld\n", N, r);
			if (r != N)
				CkAbort("test failed\n");
			else {
				CkPrintf("test verified\n");
				CkExit();
			}
		}

		void verify_contribute(CmiUInt8 r) {
			CkPrintf("N = %lld, r = %lld\n", N, r);
			if (r != N)
				CkAbort("test failed");
			else {
				CkPrintf("test verified");
				CkExit();
			}
		}
};

#include "test_contribute.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
