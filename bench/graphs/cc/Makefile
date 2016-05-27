include ../../../config.mk
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

BINARY = tram_cc charm_cc ucharelib_cc ucharelib_cc_radius

all: $(BINARY)

charm_cc: charm_cc.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_cc.o: charm_cc.C charm_cc.decl.h charm_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_cc: tram_cc.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_cc.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_cc: tramx_cc.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_cc.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_cc: ucharelib_cc.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_cc.o: ucharelib_cc.C ucharelib_cc.decl.h ucharelib_cc.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_cc_radius: ucharelib_cc_radius.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_cc_radius.o: ucharelib_cc_radius.C ucharelib_cc_radius.decl.h ucharelib_cc_radius.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
