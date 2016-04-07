INC_PATH=./graphio
LIB_PATH=./graphio
#OPTS=-std=c++11 -O3
OPTS=-std=c++11 -g -tracemode projections 

CHARMC=$(CHARM_HOME)/bin/charmc
OBJS = mst_boruvka_naive.o

all: mst_boruvka_naive

mst_boruvka_naive: $(OBJS) $(LIB_PATH)/libgraphio.a
	$(CHARMC) -language charm++ -o mst_boruvka_naive $(OBJS) -L$(LIB_PATH) -lgraphio 

mst_boruvka_naive.decl.h: mst_boruvka_naive.ci
	$(CHARMC) mst_boruvka_naive.ci 

mst_boruvka_naive.o: mst_boruvka_naive.C mst_boruvka_naive.decl.h\
graphio/GraphDistr.h\
graphio/GraphFileIO.h\
graphio/GraphGenerator.h\
graphio/graphio.h\
graphio/GraphIO.h\
graphio/InputOutput.decl.h\
graphio/InputOutput.def.h\
graphio/InputOutput.h
	$(CHARMC) -c mst_boruvka_naive.C -I$(INC_PATH)

test: test.o uChareLib.o
	$(CHARMC) -o test $^

test.o: test.C test.decl.h uChareLib.decl.h 
	$(CHARMC) -o test.o -c test.C -I. $(OPTS)

uChareLib.o: uChareLib.C uChareLib.decl.h 
	$(CHARMC) -o uChareLib.o -c uChareLib.C -I. $(OPTS)

%.decl.h: %.ci
	$(CHARMC) $< 

clean:
	rm -f *.decl.h *.def.h *.o 
	rm -f mst_boruvka_naive charmrun 

