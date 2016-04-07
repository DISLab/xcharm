include ../../config.mk
CHARMXI=../../bin/charmxi
CHARMC_COMPILE_FLAGS=
CHARMC_LINK_FLAGS=
XCHARMC_COMPILE_FLAGS=-g -I. -I../../include -I../../include/ucharelib -I../../include/NDMeshStreamer
XCHARMC_LINK_FLAGS=-L../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer -module completion -module ampi


BINARY = tram_alltoall charm_alltoall ucharelib_alltoall #tramx_alltoall 

all: $(BINARY)

charm_alltoall: charm_alltoall.o
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
charm_alltoall.o: charm_alltoall.C charm_alltoall.decl.h charm_alltoall.def.h
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -c -o $@ $<

tram_alltoall: tram_alltoall.o
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -o $@ $+ -module NDMeshStreamer
tram_alltoall.o: tram_alltoall.C tram_alltoall.decl.h tram_alltoall.def.h
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -c -o $@ $< 

tramx_alltoall: tramx_alltoall.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+ 
tramx_alltoall.o: tram_alltoall.C tram_alltoall.decl.h tram_alltoall.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

ucharelib_alltoall: ucharelib_alltoall.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ $+  
ucharelib_alltoall.o: ucharelib_alltoall.C ucharelib_alltoall.decl.h ucharelib_alltoall.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o $@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<

clean: 
	rm -f $(BINARY)
	rm -f *.decl.h *.def.h *.o charmrun 
