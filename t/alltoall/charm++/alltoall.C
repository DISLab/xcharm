#include <alltoall.decl.h>
CProxy_Main mainProxy;

int N = 100;

class Alltoall : public CBase_Alltoall {
	private:
		int counter;
	public:
		Alltoall() {
			CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
					thisIndex, thisIndex, CmiMyPe());

			contribute(CkCallback(CkReductionTarget(Main, init), mainProxy));
		}

		Alltoall(CkMigrateMessage *m) {}

		void ping(int callee) { 
			CkPrintf("[chare=%d,pe=%d]: %d: ping called by %d\n", 
					thisIndex, CmiMyPe(), thisIndex, callee);
		}

		void start() {
			CkPrintf("[chare=%d,pe=%d]: %d: start called\n", 
					thisIndex, CmiMyPe(), thisIndex);
			for (int i = 0; i < N; i++)
				thisProxy[i].ping(thisIndex);
		}
};

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		CProxy_Alltoall alltoall_proxy;

		int nElements;
		int nChares;
	public:
		Main(CkArgMsg *m) {

			alltoall_proxy = CProxy_Alltoall::ckNew(N);
			//alltoall_proxy->init();

			delete m;
		}

		void init() {
			CkStartQD(CkIndex_Main::start(), &thishandle);
		}

		void done() {
				endt = CkWallTimer();
				CkPrintf("Main: done\n");
				CkPrintf("\tnumber of chares %ld, number of sent messages %f*10^6\n", N, ((double)(N*(N-1)))/(1000000));
				CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N*(N-1)))/(1000000 *(endt-startt)));
				CkExit();
		}

		/*entry*/ void start() {
			CkPrintf("Main: run calculations...\n");
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();

			alltoall_proxy.start();
			CkPrintf("Main: finished\n");
			CkStartQD(CkIndex_Main::done(), &thishandle);
		}

		void abort() {
			CkAbort("Abort!");
		}
};

#include "alltoall.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
