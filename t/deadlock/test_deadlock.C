#include <uChareLib.h>
#include <test_deadlock.decl.h>

CProxy_Main mainProxy;
CProxy_ArrayMeshStreamer<Message, int, 
    uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall>, SimpleMeshRouter> aggregator;

class Alltoall : public CBase_Alltoall, public uChare<Alltoall, CProxy_Alltoall, CBase_Alltoall> {
	private:
		int counter;
		bool pingDone, pongDone;
	public:
		Alltoall() {
			CkAbort("Dummy constructor called");
		}
		Alltoall(std::size_t index, uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall> *uchareset) : 
			uChare<Alltoall, CProxy_Alltoall, CBase_Alltoall>(index, uchareset)  {
			CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
					getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			pingDone = pongDone = false;

			contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
		}

		void doPing(int dest) {
			CkPrintf("[chare=%d,pe=%d]: %d: doPing called, dest = %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), dest);
			if (!pingDone) {
				getProxy()[dest]->ping(getId());
				pingDone = true;
			}

			//if (pingDone && pongDone)
			//	done(CkCallback(CkIndex_Main::done(), mainProxy));
		}

		void doPong(int dest) {
			CkPrintf("[chare=%d,pe=%d]: %d: doPong called, dest = %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), dest);
			if (!pongDone) {
				getProxy()[dest]->ping(getId());
				pongDone = true;
			}

			//if (pingDone && pongDone)
			//	done(CkCallback(CkIndex_Main::done(), mainProxy));
		}

		void ping(int callee) { 
				CkPrintf("[chare=%d,pe=%d]: %d: ping called by %d\n", 
						getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
				doPong(callee);
		}

		void pong(int callee) { 
				CkPrintf("[chare=%d,pe=%d]: %d: pong called by %d\n", 
						getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
				//doPong(callee);
		}

		void start() {
			CkPrintf("[chare=%d,pe=%d]: %d: start called\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId());
			doPing((getId() + 1) % N_uChares);
		}

		void run() { 
		}

		void run(int dummy) { 
			CkAbort("run called");
		} 
};

class Main : public CBase_Main {
	private:
		CProxy_uChareSet<Alltoall, CProxy_Alltoall, CBase_Alltoall> uchareset_proxy; //FIXME:???
		double startt;
		double endt;
		Alltoall_uChareArray *alltoall_proxy;

		int nElements;
		int nChares;
	public:
		Main(CkArgMsg *m) {
			CkPrintf("%d:Hallo: argv[1] = %d, argv[2] = %d!\n", CmiMyPe(), atoi(m->argv[1]), atoi(m->argv[2]));

			nElements =  CkNumPes() * atoi(m->argv[1]);
			nChares =  CkNumPes() * atoi(m->argv[2]);

			mainProxy = thisProxy;
			//!!
			alltoall_proxy = new Alltoall_uChareArray(nElements, nChares);
			alltoall_proxy->init();

			//thisProxy.init();
			delete m;
		}

		void init() {}

		void done() {
				endt = CkWallTimer();
				CkPrintf("Main: done\n");
				CkPrintf("\tnumber of uchares %ld, number of sent messages %f*10^6\n", N_uChares, ((double)(N_uChares*(N_uChares-1)))/(1000000));
				CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N_uChares*(N_uChares-1)))/(1000000 *(endt-startt)));
				CkExit();
		}

		/*entry*/ void start() {
			CkPrintf("Main: run calculations...\n");
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();

			alltoall_proxy->run(CkCallback(CkIndex_Main::done(), thisProxy));

			//for (int i = 0; i < N_uChares; i++)
			//	(*hello_proxy)[i]->ping(0);
			alltoall_proxy->start();
			//alltoall_proxy->flush();
		}
};

#include "test_deadlock.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
