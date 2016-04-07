#include <uChareLib.h>
#include <test_pingping.decl.h>

CProxy_Main mainProxy;
CProxy_ArrayMeshStreamer<Message, int, 
    uChareSet<Pingping, CProxy_Pingping, CBase_Pingping>, SimpleMeshRouter> aggregator;

class Pingping : public CBase_Pingping, public uChare<Pingping, CProxy_Pingping, CBase_Pingping> {
	private:
		int counter;
		bool pingDone, pongDone;
		std::vector<int> pingCounters, pongCounters;

	public:
		Pingping() {
			CkAbort("Dummy constructor called");
		}
		Pingping(std::size_t index, uChareSet<Pingping, CProxy_Pingping, CBase_Pingping> *uchareset) : 
			uChare<Pingping, CProxy_Pingping, CBase_Pingping>(index, uchareset)  {
			CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
					getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			pingDone = pongDone = false;

			pingCounters.resize(N_uChares);
			pingCounters.assign(N_uChares, -1);
			pongCounters.resize(N_uChares);
			pongCounters.assign(N_uChares, 999);

			contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
		}

		void doPing(int dest, int n) {
			CkPrintf("[chare=%d,pe=%d]: %d: doPing called, dest = %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), dest);
			if (!pingDone) {
				for (int i = 0; i < n; i++) 
					getProxy()[dest]->ping(getId(), i);
				//pingDone = true;
			}

			//if (pingDone && pongDone)
			//	done(CkCallback(CkIndex_Main::done(), mainProxy));
		}

		void doPong(int dest) {
			CkPrintf("[chare=%d,pe=%d]: %d: doPong called, dest = %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), dest);
			if (!pongDone) {
				//getProxy()[dest]->ping(getId());
				pongDone = true;
			}

			//if (pingDone && pongDone)
			//	done(CkCallback(CkIndex_Main::done(), mainProxy));
		}

		void ping(int callee, int order) { 
				CkPrintf("[chare=%d,pe=%d]: %d: ping #%d called by %d\n", 
						getuChareSet()->getId(), getuChareSet()->getPe(), getId(), order, callee);
				getProxy()[callee]->pong(getId(), order+1000);

				if ((pingCounters[callee] + 1) != order) {
					CkError("Ping order failure: callee = %d, counter = %d, order = %d\n", 
							callee, pingCounters[callee], order);
					CkAbort("Ping order failure");
				}
				else
					pingCounters[callee]++;

				//doPong(callee);
		}

		void pong(int callee, int order) { 
				CkPrintf("[chare=%d,pe=%d]: %d: pong #%d called by %d\n", 
						getuChareSet()->getId(), getuChareSet()->getPe(), getId(), order, callee);
				//doPong(callee);

				if ((pongCounters[callee] + 1) != order) {
					CkError("Pong order failure: callee = %d, counter = %d, order = %d\n", 
							callee, pongCounters[callee], order);
					CkAbort("Pong order failure");
				}
				else
					pongCounters[callee]++;
		}

		void start() {
			CkPrintf("[chare=%d,pe=%d]: %d: start called\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId());
			//doPing((getId() + 1) % N_uChares, 100);

			if (getId() == 0)
				for (int i = 1; i < N_uChares; i++)
					doPing((getId() + i) % N_uChares, 100);
		}

		void run() { 
		}

		void run(int dummy) { 
			CkAbort("run called");
		} 
};

class Main : public CBase_Main {
	private:
		CProxy_uChareSet<Pingping, CProxy_Pingping, CBase_Pingping> uchareset_proxy; //FIXME:???
		double startt;
		double endt;
		Pingping_uChareArray *alltoall_proxy;

		int nElements;
		int nChares;
	public:
		Main(CkArgMsg *m) {
			CkPrintf("%d:Hallo: argv[1] = %d, argv[2] = %d!\n", CmiMyPe(), atoi(m->argv[1]), atoi(m->argv[2]));

			nElements =  CkNumPes() * atoi(m->argv[1]);
			nChares =  CkNumPes() * atoi(m->argv[2]);

			mainProxy = thisProxy;
			//!!
			alltoall_proxy = new Pingping_uChareArray(nElements, nChares);
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

#include "test_pingping.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
