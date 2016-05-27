include ../../../config.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I./include/graph500 -I$(BOOST_ROOT)/include
CHARMC_LINK_FLAGS=-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono

all: libGraphGenerator.a
	ln -sf ../src/libs/graphrt ../../../include/graphrt
	cp libGraphGenerator.a ../../../lib

libGraphGenerator.a: GraphGenerator.o
	ar r $@ $^

GraphGenerator.o: GraphGenerator.C GraphGenerator.decl.h GraphGenerator.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f libGraphGenerator.a
	rm -f *.decl.h *.def.h *.o charmrun 
