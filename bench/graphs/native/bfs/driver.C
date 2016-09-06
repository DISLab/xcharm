class TestDriver : public CBase_TestDriver {
private:
	CmiUInt8 root;
  double starttime;
	Options opts;

	BFSGraph *graph;
	typedef GraphLib::GraphGenerator<
		BFSGraph, 
		Options, 
		GraphLib::VertexMapping::SingleVertex,
		GraphLib::GraphType::Directed,
		GraphLib::GraphGeneratorType::Kronecker,
		GraphLib::TransportType::Tram> Generator;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
    driverProxy = thishandle;

    // Create the chares storing vertices
    // Create graph
    graph = new BFSGraph(CProxy_BFSVertex::ckNew(opts.N));
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

		generator->generate();

		CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void start() {
		srandom(1);
		BFSGraph::Proxy & g = graph->getProxy();
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
		root = random() % N;
    CkPrintf("start, root = %lld\n", root);
    starttime = CkWallTimer();
		graph->start(root);
		//g[root].make_root();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void restart() {
		BFSGraph::Proxy & g = graph->getProxy();
		root = random() % N;
    CkPrintf("restart, root = %lld\n", root);
    starttime = CkWallTimer();
		//g[root].make_root();
		graph->start(root);
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
  }

  void startVerificationPhase() {
		//BFSGraph::Proxy & g = graph->getProxy();
		//g.getScannedVertexNum();
		graph->getScannedVertexNum();
  }

  void done(CmiUInt8 globalNumScannedVertices) {
		BFSGraph::Proxy & g = graph->getProxy();
		CkPrintf("globalNumScannedVertices = %lld\n", globalNumScannedVertices);
		if (globalNumScannedVertices < 0.25 * N) {
			driverProxy.restart();
		} else {
			double update_walltime = CkWallTimer() - starttime;
			double gteps = 1e-9 * globalNumScannedVertices * 1.0/update_walltime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", update_walltime);
			CkPrintf("Scanned edges = %lld (%.0f%%)\n", globalNumScannedVertices, (double)globalNumScannedVertices*100/M);
			CkPrintf("%.9f Billion(10^9) Traversed edges  per second [GTEP/s]\n", gteps);
			CkPrintf("%.9f Billion(10^9) Traversed edges/PE per second [GTEP/s]\n",
							 gteps / CkNumPes());
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

