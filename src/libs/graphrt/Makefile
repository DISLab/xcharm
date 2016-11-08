include ../../../config.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I./include/graph500 -I$(BOOST_ROOT)/include -Wno-deprecated-declarations

all: libGraph.a

install: libGraph.a
	if [ ! -L ../../../include/graphrt ] ;\
	 	then ln -s ../src/libs/graphrt ../../../include/graphrt ; fi
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
