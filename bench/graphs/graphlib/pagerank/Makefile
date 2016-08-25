include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include -Wno-deprecated-declarations
CHARMC_LINK_FLAGS=-g -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi

BINARY = pagerank_charm pagerank_tram 

all: $(BINARY)

pagerank_charm: pagerank_charm.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
pagerank_charm.o: pagerank.C pagerank.decl.h pagerank.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -DGRAPH_TRANSPORT=Charm -language charm++ -c -o $@ $< 
pagerank_charm.xi: pagerank.ci
	gcc -x c++ -E -P $< -DGRAPH_TRANSPORT=Charm > $@

pagerank_tram: pagerank_tram.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
pagerank_tram.o: pagerank.C pagerank.decl.h pagerank.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -DGRAPH_TRANSPORT=Tram -language charm++ -c -o $@ $< 
pagerank_tram.xi: pagerank.ci
	gcc -x c++ -E -P $< -DGRAPH_TRANSPORT=Tram > $@

%.xi : %.ci
	gcc -x c++ -E -P $< > $@

%.def.h %.decl.h: %.xi
	$(CHARMXI) $<
	sed -i 's/CkIndex_GraphLib__/GraphLib::CkIndex_/g' $(basename $<).decl.h $(basename $<).def.h
	sed -i 's/GraphLib__/GraphLib::/g' $(basename $<).decl.h $(basename $<).def.h

clean: 
	rm -f $(BINARY)
	rm -f *.xi *.decl.h *.def.h *.o charmrun 
