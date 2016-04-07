#GRAPHHPC_FLAGS= -I./sandbox/GraphHPC-0.2 -DGRAPHHPC_CONTEST
GRAPHHPC_FLAGS= -DGRAPHHPC_CONTEST
CHARMC_FLAGS=-O2 -I../include -I../src/libs/graphio -L../lib $(GRAPHHPC_FLAGS) -std=c++11

CHARMC=$(CHARM_HOME)/bin/charmc $(CHARMC_FLAGS)

all: cd_louvain_naive


cd_louvain_naive: cd_louvain_naive.o
	$(CHARMC) -o $@ $^ -lgraphio

cd_louvain_naive.o: cd_louvain_naive.C cd_louvain_naive.def.h cd_louvain_naive.decl.h
	$(CHARMC) -c -o $@ $<

cd_louvain_naive.def.h: cd_louvain_naive.ci
	$(CHARMC) $^

clean:
	rm -f cd_louvain_naive *.o *.decl.h *.def.h
