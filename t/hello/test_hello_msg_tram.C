#include <uChareLib.h>

class Msg {
	public:
		int foo;
		int boo;
		Msg() {}
		Msg(int foo, int boo) : foo(foo), boo(boo) {}
		int size() const { return sizeof(int) * 2; }
		static void pack(const Msg *m, void *buf) {
			memcpy(buf, m, m->size());
		}
		static void unpack(Msg *m, void *buf) {
			memcpy(m, buf, m->size());
		}
};

#include <test_hello_msg_tram.decl.h>

CProxy_Main mainProxy;
CProxy_ArrayMeshStreamer<Message, int, 
    uChareSet<Hello, CProxy_Hello, CBase_Hello>, SimpleMeshRouter> aggregator;

class Hello : public CBase_Hello, public uChare<Hello, CProxy_Hello, CBase_Hello> {
	private:
		int counter;
	public:
		Hello() {
			CkAbort("Dummy constructor called");
		}
		Hello(std::size_t index, uChareSet<Hello, CProxy_Hello, CBase_Hello> *uchareset) : 
			uChare<Hello, CProxy_Hello, CBase_Hello>(index, uchareset)  {
			CkPrintf("[uchare=%d, chare=%d,pe=%d]: created \n", 
					getId(), getuChareSet()->getId(), getuChareSet()->getPe());

			contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
		}

		void ping(int callee) { 
			CkPrintf("[chare=%d,pe=%d]: %d: ping called by %d\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), callee);
		}

		void ping2(const Msg &m) { 
			CkPrintf("[chare=%d,pe=%d]: %d: ping called: foo(%d), boo(%d)\n", 
					getuChareSet()->getId(), getuChareSet()->getPe(), getId(), m.foo, m.boo);
		}

		void run() { 
			// TODO: this function must be removed
		}

		void run(int dummy) { 
			// TODO: this function must be removed
		} 
};
/**********************************************************************************************/

class Main : public CBase_Main {
	private:
		double startt;
		double endt;
		Hello_uChareArray *hello_proxy;

		int nElements;
		int nChares;

	public:
		Main(CkArgMsg *m) {
			CkPrintf("%d:Hallo: argv[1] = %d, argv[2] = %d!\n", CmiMyPe(), atoi(m->argv[1]), atoi(m->argv[2]));

			nElements =  CkNumPes() * atoi(m->argv[1]);
			nChares =  CkNumPes() * atoi(m->argv[2]);

			/**** TODO: this must be removed ****/
			//N_uChares = nElements;
			//N_uChareSets = nChares;
			/************************************/
			mainProxy = thisProxy;

			hello_proxy = new Hello_uChareArray(nElements, nChares);
			hello_proxy->init();
			//thisProxy.init();

			delete m;
		}

		void init() {
			//TODO: is it possible to move to the Main::Main?
			//hello_proxy = new Hello_uChareArray(nElements, nChares);
		}

		/*entry*/ void start() {
			CkPrintf("Main: start...\n");
			startt = CkWallTimer();
			hello_proxy->run(CkCallback(CkIndex_Main::done(), thisProxy));

			//for (int i = 0; i < N_uChares; i++)
			//	(*hello_proxy)[i]->ping(0);
			Msg m(1,2);
			hello_proxy->ping2(m);
		}

		void done() {
				endt = CkWallTimer();
				CkPrintf("Main: done\n");
				CkExit();
		}
};

#include "test_hello_msg_tram.def.h"

#define CK_TEMPLATES_ONLY
//#include "uChare"
#undef CK_TEMPLATES_ONLY
