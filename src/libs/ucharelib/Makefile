include ../../../config.mk
OPTS=-std=c++11 -O3
#OPTS=-std=c++11 -g
#OPTS=-std=c++11 -g -tracemode projections 

CHARMC=$(CHARM_HOME)/bin/charmc
OBJS = mst_boruvka_naive.o

all: libmoduleuChareLib.a

libmoduleuChareLib.a: uChareLib.o
	ar q $@ $<
	cp $@ ../../../lib/
	ln -sf ../src/libs/ucharelib ../../../include/ucharelib

uChareLib.o: uChareLib.C uChareLib.decl.h 
	$(CHARMC) -o uChareLib.o -c uChareLib.C -I. -I./NDMeshStreamer $(OPTS)

%.decl.h: %.ci
	$(CHARMC) $< 

clean:
	rm -f *.decl.h *.def.h *.o 

