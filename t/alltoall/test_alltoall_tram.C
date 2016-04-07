#include <uChareLib.h>
#include <test_alltoall_tram.decl.h>

CmiUInt8 N;
CProxy_Main mainProxy;

class Alltoall : public CBase_uChare_Alltoall {
	private:
		CmiUInt8 counter;
	public:
		Alltoall(const uChareAttr_Alltoall &attr) : counter(0), CBase_uChare_Alltoall(attr)  {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
			//		getId(), getuChareSet()->getId(), getuChareSet()->getPe());
			contribute(CkCallback(CkReductionTarget(Main, init), mainProxy));
		}

		void ping(int callee) { 
			//CkPrintf("[chare=%d,pe=%d]: %d: ping called by %d\n", 
			//		getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
			if (++counter == N) {
				contribute(CkCallback(CkReductionTarget(Main, done), mainProxy));
			}
		}

		void start() {
			//CkPrintf("[chare=%d,pe=%d]: %d: start called\n", 
			//		getuChareSet()->getId(), getuChareSet()->getPe(), getId());
			for (int i = 0; i < N_uChares; i++)
				thisProxy[i]->ping(thisIndex);
		}
};

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		CProxy_uChare_Alltoall alltoall;

	public:
		Main(CkArgMsg *m) {
			if (m->argc > 1) {
				N = CkNumPes() * (1 << atoi(m->argv[1]));
			} else {
				// default values for nElements and nChares
				N = CkNumPes() * (1 << 4);
			}

			CkPrintf("Alltoall test started:\n");
			CkPrintf("uchares = %lld, chare sets per PE = %d\n", N, 1);
			mainProxy = thisProxy;
			alltoall = CProxy_uChare_Alltoall::ckNew(N, CkNumPes());

			delete m;
		}

		void init() {
			alltoall.run(CkCallback(CkIndex_Main::start(), thisProxy));
		}

		void done() {
				endt = CkWallTimer();
				CkPrintf("Main: done\n");
				CkPrintf("\tnumber of uchares %ld, number of sent messages %f*10^6\n", N_uChares, ((double)(N_uChares*(N_uChares-1)))/(1000000));
				CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N_uChares*(N_uChares-1)))/(1000000 *(endt-startt)));
				CkExit();
		}

		/*entry*/ void start() {
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();
			alltoall.start();
			alltoall.flush();
			//CkStartQD(CkIndex_Main::done(), &thishandle);
		}

		void abort() {
			CkAbort("Abort!");
		}
};

#include "test_alltoall_tram.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
