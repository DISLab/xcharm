CHARMC=../../../bin/charmc 
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

BINARY = tram_sssp charm_sssp ucharelib_sssp ucharelib_sssp_radius

all: $(BINARY)

charm_sssp: charm_sssp.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_sssp.o: charm_sssp.C charm_sssp.decl.h charm_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_sssp: tram_sssp.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_sssp.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_sssp: tramx_sssp.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_sssp.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_sssp: ucharelib_sssp.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_sssp.o: ucharelib_sssp.C ucharelib_sssp.decl.h ucharelib_sssp.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_sssp_radius: ucharelib_sssp_radius.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_sssp_radius.o: ucharelib_sssp_radius.C ucharelib_sssp_radius.decl.h ucharelib_sssp_radius.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
