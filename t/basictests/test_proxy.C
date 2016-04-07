#include <uChareLib.h>
#include <proxytest.decl.h>

CmiUInt8 N, M;
CProxy_Main mainProxy;

class Consumer : public CBase_uChare_Consumer {
	public:
		CProxy_Consumer consumer;
		Consumer(const uChareAttr_Consumer &attr) : CBase_uChare_Consumer(attr) {
			//CkPrintf("[uchare=%d, chare=%d,pe=%d]: consumer created \n", 
			//		thisIndex, getuChareSet()->getId(), getuChareSet()->getPe());
		}
		void recv (int callee) {
			//CkPrintf("[chare=%d,pe=%d]: Consumer %lld: recv called by Producer %d\n", 
			//		getuChareSet()->getId(), getuChareSet()->getPe(), thisIndex, callee);
		}
};

class Producer : public CBase_Producer {
	public:
		CProxy_uChare_Consumer consumers;
		Producer(CProxy_uChare_Consumer &consumers) : consumers(consumers) {
			//CkPrintf("[chare=%d,pe=%d]: producer created \n", 
			//		thisIndex, CmiMyPe());
		}
		Producer(CkMigrateMessage *m) {}
		void send () {
			for (int i = 0; i < N; i++)
				consumers[i]->recv(thisIndex);
		}
};

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		CProxy_uChare_Consumer consumers;
		CProxy_Producer producers;

	public:
		Main(CkArgMsg *m) {
			if (m->argc > 2) {
				N = CkNumPes() * (1 << atoi(m->argv[1]));
				M = CkNumPes() * (1 << atoi(m->argv[2]));
			} else {
				// default values for nElements and nChares
				N = CkNumPes() * (1 << 4);
				M = CkNumPes() * (1 << 1);
			}

			CkPrintf("Producer/Consumer test started:\n");
			CkPrintf("uchares = %lld, chare sets per PE = %d\n", N, 1);
			CkPrintf("N = %lld, M = %lld\n", N, M);
			mainProxy = thisProxy;
			consumers = CProxy_uChare_Consumer::ckNew(N, CkNumPes());
			producers = CProxy_Producer::ckNew(consumers, M);

			CkStartQD(CkIndex_Main::init(), &thishandle);

			delete m;
		}

		void init() {
			CkPrintf("Main: init...\n");
			//CkExit();
			consumers.run(CkCallback(CkIndex_Main::start(), thisProxy));
		}

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
			producers.send();
			CkStartQD(CkIndex_Main::done(), &thishandle);
		}

		void abort() {
			CkAbort("Abort!");
		}
};

#include "proxytest.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
