#ifndef __common_h__
#define __common_h__

struct Options {
	int scale;
	int K;
	bool strongscale;
	bool verify;
	CmiUInt8 N;
	CmiUInt8 M;
	CmiUInt8 root;
	Options() : scale(10), K(16), strongscale(true), verify(false), root(0) {
		N = (1 << scale);
		M = N * K;
	}
	void pup(PUP::er & p) {
		p | scale;
		p | K;
		p | strongscale;
		p | verify;
		p | N;
		p | M;
		p | root;
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
	}

	if (opts.strongscale)
		opts.N = (1 << opts.scale);
	else
		opts.N = (1 << opts.scale) * CkNumPes();
	opts.M = opts.N * opts.K;
}

#endif
