include ../../../config.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I.. -I../../../include/graphrt  -I../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include
CHARMC_LINK_FLAGS=-L../../../lib -lGraphGenerator \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi

XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_pagerank_naive charm_pagerank_intracount charm_pagerank_async \
				 tram_pagerank ucharelib_pagerank ucharelib_pagerank_radius

all: $(BINARY)

charm_pagerank_naive: charm_pagerank_naive.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_pagerank_naive.o: charm_pagerank_naive.C charm_pagerank_naive.decl.h charm_pagerank_naive.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

charm_pagerank_intracount: charm_pagerank_intracount.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_pagerank_intracount.o: charm_pagerank_intracount.C charm_pagerank_intracount.decl.h charm_pagerank_intracount.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

charm_pagerank_async: charm_pagerank_async.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_pagerank_async.o: charm_pagerank_async.C charm_pagerank_async.decl.h charm_pagerank_async.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_pagerank: tram_pagerank.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_pagerank.o: tram_pagerank.C tram_pagerank.decl.h tram_pagerank.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_pagerank: tramx_pagerank.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_pagerank.o: tram_pagerank.C tram_pagerank.decl.h tram_pagerank.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_pagerank: ucharelib_pagerank.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_pagerank.o: ucharelib_pagerank.C ucharelib_pagerank.decl.h ucharelib_pagerank.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_pagerank_radius: ucharelib_pagerank_radius.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_pagerank_radius.o: ucharelib_pagerank_radius.C ucharelib_pagerank_radius.decl.h ucharelib_pagerank_radius.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
