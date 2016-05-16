include ../../../config.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I.. -I../../../include/graphrt  -I../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS)
CHARMC_LINK_FLAGS=-L../../../lib -lGraphGenerator \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = tram_bfs_async charm_bfs_async charm_bfs_sync ucharelib_bfs_async ucharelib_bfs_async_radix

all: $(BINARY)

charm_bfs_async: charm_bfs_async.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_bfs_async.o: charm_bfs_async.C charm_bfs_async.decl.h charm_bfs_async.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

charm_bfs_sync: charm_bfs_sync.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_bfs_sync.o: charm_bfs_sync.C charm_bfs_sync.decl.h charm_bfs_sync.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_bfs_async: tram_bfs_async.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_bfs_async.o: tram_bfs_async.C tram_bfs_async.decl.h tram_bfs_async.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_bfs_async: tramx_bfs_async.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_bfs_async.o: tram_bfs_async.C tram_bfs_async.decl.h tram_bfs_async.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_bfs_async: ucharelib_bfs_async.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_bfs_async.o: ucharelib_bfs_async.C ucharelib_bfs_async.decl.h ucharelib_bfs_async.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_bfs_async_radix: ucharelib_bfs_async_radix.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_bfs_async_radix.o: ucharelib_bfs_async_radix.C ucharelib_bfs_async_radix.decl.h ucharelib_bfs_async_radix.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
