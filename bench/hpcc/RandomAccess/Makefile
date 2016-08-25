include ../../../config2.mk
CHARMXI=../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS)
CHARMC_LINK_FLAGS=-L./.tmp -L../../../../lib \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-g -I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer
XCHARMC_LINK_FLAGS=-L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer -module completion -module ampi


BINARY = tram_randomAccess charm_randomAccess #ucharelib_randomAccess #tramx_randomAccess 

all: $(BINARY)

charm_randomAccess: charm_randomAccess.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_randomAccess.o: charm_randomAccess.C charm_randomAccess.decl.h charm_randomAccess.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $<

tram_randomAccess: tram_randomAccess.o
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_randomAccess.o: tram_randomAccess.C tram_randomAccess.decl.h tram_randomAccess.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

tramx_randomAccess: tramx_randomAccess.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_randomAccess.o: tram_randomAccess.C tram_randomAccess.decl.h tram_randomAccess.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_randomAccess: ucharelib_randomAccess.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_randomAccess.o: ucharelib_randomAccess.C ucharelib_randomAccess.decl.h ucharelib_randomAccess.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
