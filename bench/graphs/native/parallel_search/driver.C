class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime, totaltime;
	Options opts;

	BFSGraph *graph;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
    driverProxy = thishandle;

    // Create the chares storing vertices
    // Create graph
    graph = new BFSGraph(N);
		// create graph generator
		generator = new Generator(*graph, opts);

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("BFS running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();
		generator->generate(CkCallback(CkIndex_TestDriver::start(), thishandle));
		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
		srandom(1);
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
    CkPrintf("start, root = %lld\n", root);
    starttime = CkWallTimer();
		totaltime = 0;
		graph->start(root);
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		root = random() % N;
    CkPrintf("restart, root = %lld\n", root);
    starttime = CkWallTimer();
		graph->start(root);
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		totaltime += CkWallTimer() - starttime;
		graph->getScannedVertexNum();
  }

  void done(CmiUInt8 totalScannedVertices) {
		CkPrintf("totalScannedVertices = %lld\n", totalScannedVertices);
		if (totalScannedVertices < 0.25 * N) {
			driverProxy.restart();
		} else {
			double totaltime = CkWallTimer() - starttime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", totaltime);
			CkPrintf("Scanned edges = %lld (%.0f%%)\n", totalScannedVertices, (double)totalScannedVertices*100/M);
			if (opts.verify) { 
				CkPrintf("Start verification...\n");
				graph->verify();
			}
			CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
		}
  }

	void exit() {
		CkPrintf("Done.\n");
		CkExit();
	}
};

