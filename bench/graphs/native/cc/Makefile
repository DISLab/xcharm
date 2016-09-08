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

BINARY = charm_cc \
				 charm_multivertex_cc \
				 tram_cc_r1 tram_cc_r16 tram_cc_r32 tram_cc_r64 \
				 tram_cc_r256 tram_cc_r512 tram_cc_r1024 \
				 tram_multivertex_cc
#				 ucharelib_cc_async ucharelib_cc_async_radix

all: dir $(BINARY)

charm_cc: charm_cc.o
charm_cc.o: charm_cc.C charm_cc.decl.h charm_cc.def.h

charm_cc_radix: charm_cc_radix.o
charm_cc_radix.o: charm_cc_radix.C charm_cc_radix.decl.h charm_cc_radix.def.h

tram_cc_r1: tram_cc_r1.o
tram_cc_r1.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=1 

tram_cc_r16: tram_cc_r16.o
tram_cc_r16.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=16 

tram_cc_r32: tram_cc_r32.o
tram_cc_r32.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=32

tram_cc_r64: tram_cc_r64.o
tram_cc_r64.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_cc_r256: tram_cc_r256.o
tram_cc_r256.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=256

tram_cc_r512: tram_cc_r512.o
tram_cc_r512.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=512

tram_cc_r1024: tram_cc_r1024.o
tram_cc_r1024.o: tram_cc.C tram_cc.decl.h tram_cc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=1024

charm_multivertex_cc: charm_multivertex_cc.o
charm_multivertex_cc.o: charm_multivertex_cc.C charm_multivertex_cc.decl.h charm_multivertex_cc.def.h

tram_multivertex_cc: tram_multivertex_cc.o
tram_multivertex_cc.o: tram_multivertex_cc.C tram_multivertex_cc.decl.h tram_multivertex_cc.def.h

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
