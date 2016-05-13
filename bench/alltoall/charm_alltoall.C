#include "charm_alltoall.decl.h"

CmiUInt8 N = (1<<12);
int K = 16;

CProxy_TestDriver driverProxy;

class TestDriver : public CBase_TestDriver {
	private:
		CProxy_Alltoall alltoall_array;
		double startt, endt;

	public:
		TestDriver(CkArgMsg *m) {
			if (m->argc > 1)
				N = (1 << atoi(m->argv[1])) * CkNumPes();
			alltoall_array = CProxy_Alltoall::ckNew(N);
		}
		/*entry*/ void init() {
		  alltoall_array.run();
			startt = CkWallTimer(); 
			CkStartQD(CkIndex_TestDriver::done(), &thishandle);
		}
		/*entry*/ void done() {
			CkPrintf("TestDriver::done\n");
			endt = CkWallTimer();
			CkPrintf("\tnumber of chares %lld, number of sent messages %f*10^6\n", N, ((double)(N*(N-1)))/(1000000));
			CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N*(N-1)))/(1000000 *(endt-startt)));
			CkExit();
		}
};

class Alltoall : public CBase_Alltoall {
	private:
		CmiUInt8 counter;
	public:
		Alltoall() : counter(1) {
			contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
		}
		Alltoall(CkMigrateMessage *m){}

		/*entry*/ void run() {
			for (CmiUInt8 i = 0; i < K; i++)
				if (i != thisIndex) thisProxy[rand() % N].ping();
		}

		/*entry*/ void ping() {
			counter++;
			//CkPrintf("%d:counter = %lld, N = %lld\n", thisIndex, counter, N);
		}
};

#include "charm_alltoall.def.h"
