#ifndef __common_h__
#define __common_h__

struct Options;
void parseCommandOptions(int argc, char **argv, Options & opts);

struct Options {
	int scale;
	int K;
	int R;
	bool strongscale;
	bool verify;
	bool freeze;
	bool freeze_after_graph_creation;
	CmiUInt8 N;
	CmiUInt8 M;
	CmiUInt8 root;

	struct SSCA_Options {
		CmiUInt8 maxCliqueSize;
		bool foreward, backward;
		int numParallelEdges;
		SSCA_Options() : maxCliqueSize(8), foreward(true), backward(false), numParallelEdges(1) {}

		void pup(PUP::er & p) {
			p | maxCliqueSize;
			p | foreward;
			p | backward;
			p | numParallelEdges;
		}
	};
	SSCA_Options ssca;


	Options() : scale(10), K(16), strongscale(true), verify(false), 
		freeze(false), freeze_after_graph_creation(false), root(0) {
		N = (1 << scale);
		M = N * K;
	}
	Options(int argc, char **argv) {
		parseCommandOptions(argc, argv, *this);
	}

	void pup(PUP::er & p) {
		p | scale;
		p | K;
		p | strongscale;
		p | verify;
		p | N;
		p | M;
		p | R;
		p | root;
		p | ssca;
	}
};

void usage(int argc, char **argv) {
		printf("Usage:\n");
		printf("Options:\n");
		CkExit();
}

void parseCommandOptions(int argc, char **argv, Options & opts)
{
	if (argc == 1) usage(argc, argv);

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--scale") || !strcmp(argv[i], "-s")) {
			opts.scale = (int) atoi(argv[++i]);
		}
		if (!strcmp(argv[i], "--strongscale") || !strcmp(argv[i], "--strong")) {
			opts.strongscale = true;
		}
		if (!strcmp(argv[i], "--weakscale") || !strcmp(argv[i], "--weak")) {
			opts.strongscale = false;
		}
		if (!strcmp(argv[i], "--verify") || !strcmp(argv[i], "-v")) {
			opts.verify = true;
		}
		if (!strcmp(argv[i], "--freeze")) {
			opts.freeze = true;
		}
		if (!strcmp(argv[i], "--freeze-after-graph-creation")) {
			opts.freeze_after_graph_creation = true;
		}
		if (!strcmp(argv[i], "--max-clique-size") || !strcmp(argv[i], "-c")) {
			opts.ssca.maxCliqueSize = (int) atoi(argv[++i]);
		}
		if (!strcmp(argv[i], "--radix") || !strcmp(argv[i], "-r")) {
			opts.ssca.maxCliqueSize = (int) atoi(argv[++i]);
		}
	}

	if (opts.strongscale)
		opts.N = (1 << opts.scale);
	else
		opts.N = (1 << opts.scale) * CkNumPes();
	opts.M = opts.N * opts.K;

	while(opts.freeze) {}
}

#include <boost/random/linear_congruential.hpp>

class RootSelector {
	boost::rand48 gen;
	boost::uniform_int<uint64_t> rand_vertex;

public:
	RootSelector(Options & opts) :
		rand_vertex(0, opts.N-1) {}
	CmiUInt8 select() { return rand_vertex(gen); }
};



#endif
