#include <uChareLib.h>

typedef CmiUInt8 dtype;

//TODO: modify xi-lat to add variable message size support
//#include "ucharelib_randomAccess.decl.h"
#include "ucharelib_randomAccess.decl.h"
#include "TopoManager.h"

#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL

// log_2 of the table size per PE
int N;

// The local table size
CmiInt8 localTableSize;

// Handle to the test driver (chare)
CProxy_TestDriver driverProxy;

// Number of uchares per PE
long long numElementsPerPe;

// Number of chares per PE
int numChareSetsPerPe;

CmiUInt8 HPCC_starts(CmiInt8 n);

//
// Test driver (mainchare) 
//
class TestDriver : public CBase_TestDriver {
private:
	CProxy_uChare_Updater array;
  double starttime;
  CmiInt8 tableSize;

public:
  TestDriver(CkArgMsg* args) {
    N = atoi(args->argv[1]);
    numElementsPerPe = atoll(args->argv[2]);
		numChareSetsPerPe = CkNumPes(); //atoi(args->argv[3]);

    localTableSize = (1l << N) / numElementsPerPe;
		if (!localTableSize)
			CkAbort("Table size is too small, or number of chares is too large\n");

    tableSize = localTableSize * CkNumPes() * numElementsPerPe;

    CkPrintf("Local table size   = %lld words\n",
             localTableSize);
    CkPrintf("Global table size   = 2^%d * %d = %lld words\n",
             N, CkNumPes(), tableSize);
    CkPrintf("Number of processors = %d\nNumber of updates = %lld\n",
             CkNumPes(), 4 * tableSize);
    CkPrintf("Number of uchares per Pe = %d\nNumber of chares (uChareSets) per Pe = %d\n",
             numElementsPerPe, numChareSetsPerPe);

    driverProxy = thishandle;
		array = CProxy_uChare_Updater::ckNew(numElementsPerPe * CkNumPes(), CkNumPes());

    delete args;
  }

	void init() {
		array.run(CkCallback(CkIndex_TestDriver::start(), driverProxy));
	}

  void start() {
		CkPrintf("TestDriver::start called\n");
		starttime = CkWallTimer();
		array.generateUpdates();
		//array.flush();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		CkPrintf("startVerificationPhase called");
    double update_walltime = CkWallTimer() - starttime;
    double gups = 1e-9 * tableSize * 4.0/update_walltime;
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    CkPrintf("%.9f Billion(10^9) Updates    per second [GUP/s]\n", gups);
    CkPrintf("%.9f Billion(10^9) Updates/PE per second [GUP/s]\n",
             gups / CkNumPes());
		CkPrintf("Main: done\n");
		CkPrintf("\tnumber of uchares %ld\n", N_uChares);

    // Repeat the update process to verify
    // At the end of the second update phase, check the global table
    //  for errors in Updater::checkErrors()
		array.generateUpdates();
		//array.flush();
		CkStartQD(CkIndex_TestDriver::checkErrors(), &thishandle);
  }

	void checkErrors() {
		array.checkErrors();
		array.flush();
	}

  void reportErrors(CmiInt8 globalNumErrors) {
    CkPrintf("Found %lld errors in %lld locations (%s).\n", globalNumErrors,
             tableSize, globalNumErrors <= 0.01 * tableSize ?
             "passed" : "failed");
    CkExit();
  }

	void done() {
		CkExit();
	}
};

// Each uchare: owns a portion of the global table
//             performs updates on its portion
//             generates random keys and sends them to the appropriate chares
class Updater : public CBase_uChare_Updater {
	private:
		CmiUInt8 *HPCC_Table;
		CmiUInt8 globalStartmyProc;
	public:
		Updater(const uChareAttr_Updater &attr) : CBase_uChare_Updater(attr) {
			// Compute table start for this chare
			globalStartmyProc = thisIndex * localTableSize;
			// Create table;
			HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
			// Initialize
			for(CmiInt8 i = 0; i < localTableSize; i++)
				HPCC_Table[i] = i + globalStartmyProc;
			// Contribute to a reduction to signal the end of the setup phase
			contribute(CkCallback(CkReductionTarget(TestDriver, init), driverProxy));
		}

		// Communication library calls this to deliver each randomly generated key
		inline void update(const CmiUInt8  &key) {
			//CkPrintf("%lld: update\n", thisIndex);
			CmiInt8  localOffset = key & (localTableSize - 1);
			// Apply update
			HPCC_Table[localOffset] ^= key;
		}

		void generateUpdates() {
			int arrayN = N - (int) log2((double) numElementsPerPe);
			int numElements = CkNumPes() * numElementsPerPe;
			CmiUInt8 key = HPCC_starts(4 * globalStartmyProc);

			// Generate this chare's share of global updates
			for(CmiInt8 i = 0; i < 4 * localTableSize; i++) {
				key = key << 1 ^ ((CmiInt8) key < 0 ? POLY : 0);
				int destinationIndex = key >> arrayN & numElements - 1;
				// Submit generated key to chare owning that portion of the table
				thisProxy[destinationIndex]->update(key);
				//CkPrintf("generateUpdates: destinationIndex = %d\n", destinationIndex);
			}

			// Indicate to the uchareset library that this uchare is done
			//done();
			//getProxy().flush();

		}

		void checkErrors() {
			CmiInt8 numErrors = 0;
			// The second verification phase should have returned the table
			//  to its initial state
			for (CmiInt8 j = 0; j < localTableSize; j++)
				if (HPCC_Table[j] != j + globalStartmyProc)
					numErrors++;
			// Sum the errors observed across the entire system
			contribute(sizeof(CmiInt8), &numErrors, CkReduction::sum_long,
					CkCallback(CkReductionTarget(TestDriver, reportErrors),
						driverProxy));
		}

		/*void run() {
			CkAbort("Updater: run() called");
		}

		void run(int phase) { 
			if (phase == 0)
				generateUpdates();
			else
				checkErrors();
		} */
};

/** random number generator */
CmiUInt8 HPCC_starts(CmiInt8 n) {
  int i, j;
  CmiUInt8 m2[64];
  CmiUInt8 temp, ran;
  while (n < 0) n += PERIOD;
  while (n > PERIOD) n -= PERIOD;
  if (n == 0) return 0x1;
  temp = 0x1;
  for (i = 0; i < 64; i++) {
    m2[i] = temp;
    temp = temp << 1 ^ ((CmiInt8) temp < 0 ? POLY : 0);
    temp = temp << 1 ^ ((CmiInt8) temp < 0 ? POLY : 0);
  }
  for (i = 62; i >= 0; i--)
    if (n >> i & 1)
      break;

  ran = 0x2;
  while (i > 0) {
    temp = 0;
    for (j = 0; j < 64; j++)
      if (ran >> j & 1)
        temp ^= m2[j];
    ran = temp;
    i -= 1;
    if (n >> i & 1)
      ran = ran << 1 ^ ((CmiInt8) ran < 0 ? POLY : 0);
  }
  return ran;
}

#include "ucharelib_randomAccess.def.h"
