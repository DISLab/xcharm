include ../../../config.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I./include/graph500 -I$(BOOST_ROOT)/include

all: libGraph.a
	ln -sf ../src/libs/graphrt ../../../include/graphrt
	cp libGraph.a ../../../lib

libGraph.a: Graph.o
	ar r $@ $^

Graph.o: Graph.C Graph.decl.h Graph.def.h GraphGenerator.decl.h GraphGenerator.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f libGraph.a
	rm -f *.decl.h *.def.h *.o charmrun 
