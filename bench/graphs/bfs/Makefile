CHARMC=../../../bin/charmc 
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -O3 -I.. -I../../../include/graphrt  -I../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include
CHARMC_LINK_FLAGS=-L../../../lib -lGraphGenerator \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = tram_bfs charm_bfs ucharelib_bfs ucharelib_bfs_radius

all: $(BINARY)

charm_bfs: charm_bfs.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_bfs.o: charm_bfs.C charm_bfs.decl.h charm_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_bfs: tram_bfs.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_bfs.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_bfs: tramx_bfs.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_bfs.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_bfs: ucharelib_bfs.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_bfs.o: ucharelib_bfs.C ucharelib_bfs.decl.h ucharelib_bfs.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_bfs_radius: ucharelib_bfs_radius.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_bfs_radius.o: ucharelib_bfs_radius.C ucharelib_bfs_radius.decl.h ucharelib_bfs_radius.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
