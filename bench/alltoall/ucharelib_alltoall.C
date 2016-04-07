#include "uChareLib.h"
#include "ucharelib_alltoall.decl.h"

CmiUInt8 N = (1<<12);
CProxy_TestDriver driverProxy;

class TestDriver : public CBase_TestDriver {
	private:
		CProxy_uChare_Alltoall alltoall_array;
		double startt, endt;

	public:
		TestDriver(CkArgMsg *m) {
			if (m->argc > 1)
				N = (1 << atoi(m->argv[1])) * CkNumPes();
			alltoall_array = CProxy_uChare_Alltoall::ckNew(N, CkNumPes());
			delete m;
		}
		/*entry*/ void init() {
			alltoall_array.run(CkCallback(CkIndex_TestDriver::start(), thisProxy));
		}
		/*entry*/ void start() {
		  alltoall_array.start();
			startt = CkWallTimer(); 
		}
		/*entry*/ void done() {
			CkPrintf("TestDriver::done\n");
			endt = CkWallTimer();
			CkPrintf("\tnumber of chares %lld, number of sent messages %f*10^6\n", N, ((double)(N*(N-1)))/(1000000));
			CkPrintf("\t%f secs, %f.4 MMsg/s \n", endt-startt, ((double)(N*(N-1)))/(1000000 *(endt-startt)));
			CkExit();
		}
};

class Alltoall : public CBase_uChare_Alltoall {
	private:
		CmiUInt8 counter;
	public:
		Alltoall(const uChareAttr_Alltoall & attr) : counter(1), CBase_uChare_Alltoall(attr) {
			contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
		}

		///*entry*/ void run() {
		/*entry*/ void start() {
			for (CmiUInt8 i = 0; i < N; i++)
				if (i != thisIndex) thisProxy[i]->ping();
		}

		/*entry*/ void ping() {
			if (++counter == N)
				contribute(CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
			//CkPrintf("%d:counter = %lld, N = %lld\n", thisIndex, counter, N);
		}
};

#include "ucharelib_alltoall.def.h"
