include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -O3 -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS) -Wno-deprecated-declarations
CHARMC_LINK_FLAGS=-L./.tmp -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_sssp \
				 tram_sssp_r1 tram_sssp_r16 tram_sssp_r32 tram_sssp_r64 \
				 tram_sssp_r256 tram_sssp_r512 tram_sssp_r1024
#				 charm_multivertex_sssp
#				 tram_sssp tram_sssp_radix 
#				 ucharelib_sssp_async ucharelib_sssp_async_radix

all: dir $(BINARY)

charm_sssp: charm_sssp.o
charm_sssp.o: charm_sssp.C charm_sssp.decl.h charm_sssp.def.h

charm_sssp_radix: charm_sssp_radix.o
charm_sssp_radix.o: charm_sssp_radix.C charm_sssp_radix.decl.h charm_sssp_radix.def.h

tram_sssp_r1: tram_sssp_r1.o
tram_sssp_r1.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=1 

tram_sssp_r16: tram_sssp_r16.o
tram_sssp_r16.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=16 

tram_sssp_r32: tram_sssp_r32.o
tram_sssp_r32.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=32

tram_sssp_r64: tram_sssp_r64.o
tram_sssp_r64.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_sssp_r256: tram_sssp_r256.o
tram_sssp_r256.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_sssp_r512: tram_sssp_r512.o
tram_sssp_r512.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_sssp_r1024: tram_sssp_r1024.o
tram_sssp_r1024.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_sssp: tram_sssp.o
tram_sssp.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h

tram_sssp_radix: tram_sssp_radix.o
tram_sssp_radix.o: tram_sssp_radix.C tram_sssp_radix.decl.h tram_sssp_radix.def.h

charm_multivertex_sssp: charm_multivertex_sssp.o
charm_multivertex_sssp.o: charm_multivertex_sssp.C charm_multivertex_sssp.decl.h charm_multivertex_sssp.def.h

tram_multivertex_sssp: tram_multivertex_sssp.o
tram_multivertex_sssp.o: tram_multivertex_sssp.C tram_multivertex_sssp.decl.h tram_multivertex_sssp.def.h

%: %.o 
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ .tmp/$< -module NDMeshStreamer

%.o: %.C %.decl.h %.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

%.def.h %.decl.h: %.ci
	$(CHARMXI) $<
	sed -i 's/CkIndex_GraphLib__/GraphLib::CkIndex_/g' $(basename $<).decl.h $(basename $<).def.h
	sed -i 's/GraphLib__/GraphLib::/g' $(basename $<).decl.h $(basename $<).def.h
	mv *.decl.h .tmp && mv *.def.h .tmp

clean: 
	rm -f $(BINARY)
	rm -f .tmp/*.decl.h .tmp/*.def.h .tmp/*.o charmrun 

clean-all:
	rm -rf .tmp

.PHONY: dir

dir:
	mkdir -p .tmp
