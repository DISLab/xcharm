#include "NDMeshStreamer.h"

typedef CmiUInt8 dtype;
#include "tram_alltoall.decl.h"

CmiUInt8 N = (1<<12);
int K = 16;

CProxy_TestDriver driverProxy;
CProxy_ArrayMeshStreamer<dtype, int, Alltoall, SimpleMeshRouter> aggregator;
const int numMsgsBuffered = 1024;

class TestDriver : public CBase_TestDriver {
	private:
		CProxy_Alltoall alltoall_array;
		double startt, endt;

	public:
		TestDriver(CkArgMsg *m) {
			if (m->argc > 1)
				N = (1 << atoi(m->argv[1])) * CkNumPes();
			alltoall_array = CProxy_Alltoall::ckNew(N);

			int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
			CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

			// Instantiate communication library group with a handle to the client
			aggregator =
				CProxy_ArrayMeshStreamer<dtype, int, Alltoall, SimpleMeshRouter>
				::ckNew(numMsgsBuffered, 2, dims, alltoall_array, 1);
			delete m;
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
			ArrayMeshStreamer<dtype, int, Alltoall, SimpleMeshRouter>
				* localAggregator = aggregator.ckLocalBranch();
			for (CmiUInt8 i = 0; i < K; i++)
				if (i != thisIndex) localAggregator->insertData(0,i); 
		}

		/*entry*/ void ping() {
			counter++;
			//CkPrintf("%d:counter = %lld, N = %lld\n", thisIndex, counter, N);
		}

		inline void process(const dtype  &key) {
			ping();
		}
};

#include "tram_alltoall.def.h"
