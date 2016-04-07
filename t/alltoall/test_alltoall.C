#include <uChareLib.h>
#include <test_alltoall.decl.h>

#define N (1<<12)
#define M (1<<1)

CProxy_Main mainProxy;

class Alltoall : public CBase_Alltoall, public uChare<Alltoall, CProxy_Alltoall, CBase_Alltoall> {
	private:
		int counter;
	public:
		Alltoall() : counter(1) {}
		Alltoall(std::size_t index, uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall> *uchareset) : 
			uChare<Alltoall, CProxy_Alltoall, CBase_Alltoall>(index, uchareset)  {}

		void ping(int callee) { 
			//CkPrintf("[chare=%d,pe=%d]: %d: ping called by %d\n", 
			//		getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
			if (++counter == N_uChares) {
				//CkPrintf("[chare=%d,pe=%d]: %d: alltoall done\n", 
				//		getuChareSet()->getId(), getuChareSet()->getPe(), getId());
				mainProxy.done();
			}
		}

		void run() { CkAbort("run() is called"); }

		void run(int dummy) { 
			for (int i = 0; i < N_uChares; i++)
				getProxy()[i]->ping(getId());
		} 
};

class Main : public CBase_Main {
	private:
		std::size_t completed;
		CProxy_uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall> uchare_set_proxy;
		double startt;
		double endt;
	public:
		Main(CkArgMsg *m) : completed(0) {
			CkPrintf("%d:Hallo: argv[1] = %d, argv[2] = %d!\n", CmiMyPe(), atoi(m->argv[1]), atoi(m->argv[2]));
			//N_uChareSets = M;
			//N_uChares = N;
			N_uChareSets = atoi(m->argv[1]);
			N_uChares = atoi(m->argv[2]);
			mainProxy = thisProxy;

			CkCallback cb(CkIndex_Main::reportInitializationCompletion(), thisProxy);
		  uchare_set_proxy = CProxy_uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall>::ckNew(cb, N_uChareSets);
		}

		void reportInitializationCompletion() {
			if (++completed == N_uChareSets) {
				CkPrintf("Main: initialization completed (distributed graph created)\n");
				CkPrintf("Main: run calculations...\n");
				completed = 0;
				mainProxy.run();
			}
		}

		void done() {
			//	CkPrintf("Main: done called %d\n", completed);
			if (++completed == N_uChares) {
				endt = CkWallTimer();
				CkPrintf("Main: done\n");
				CkPrintf("\tnumber of uchares %ld, number of sent messages %f*10^6\n", N_uChares, ((double)(N_uChares*(N_uChares-1)))/(1000000));
				CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N_uChares*(N_uChares-1)))/(1000000 *(endt-startt)));
				CkExit();
			}
		}

		/*entry*/ void run() {
			startt = CkWallTimer();
			uchare_set_proxy.run(CkCallbackResumeThread());
			while(1){
				//CkPrintf("Main: run resumed...\n");
				uchare_set_proxy.flush(CkCallbackResumeThread());
				sleep(0.1);
			}
		}
};

#include "test_alltoall.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChareSet.def.h"
#undef CK_TEMPLATES_ONLY
