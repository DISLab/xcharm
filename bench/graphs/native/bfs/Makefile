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

BINARY = charm_bfs \
				 charm_multivertex_bfs \
				 tram_bfs_r1 tram_bfs_r16 tram_bfs_r32 tram_bfs_r64 \
				 tram_bfs_r256 tram_bfs_r512 tram_bfs_r1024 \
				 tram_bfs \
				 tram_multivertex_bfs
#				 ucharelib_bfs_async ucharelib_bfs_async_radix

all: dir $(BINARY)

charm_bfs: charm_bfs.o
charm_bfs.o: charm_bfs.C charm_bfs.decl.h charm_bfs.def.h

charm_bfs_radix: charm_bfs_radix.o
charm_bfs_radix.o: charm_bfs_radix.C charm_bfs_radix.decl.h charm_bfs_radix.def.h

tram_bfs: tram_bfs.o
tram_bfs.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h

tram_bfs_radix: tram_bfs_radix.o
tram_bfs_radix.o: tram_bfs_radix.C tram_bfs_radix.decl.h tram_bfs_radix.def.h

tram_bfs_r1: tram_bfs_r1.o
tram_bfs_r1.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=1 

tram_bfs_r16: tram_bfs_r16.o
tram_bfs_r16.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=16 

tram_bfs_r32: tram_bfs_r32.o
tram_bfs_r32.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=32

tram_bfs_r64: tram_bfs_r64.o
tram_bfs_r64.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_bfs_r256: tram_bfs_r256.o
tram_bfs_r256.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_bfs_r512: tram_bfs_r512.o
tram_bfs_r512.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_bfs_r1024: tram_bfs_r1024.o
tram_bfs_r1024.o: tram_bfs.C tram_bfs.decl.h tram_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

charm_multivertex_bfs: charm_multivertex_bfs.o
charm_multivertex_bfs.o: charm_multivertex_bfs.C charm_multivertex_bfs.decl.h charm_multivertex_bfs.def.h

tram_multivertex_bfs: tram_multivertex_bfs.o
tram_multivertex_bfs.o: tram_multivertex_bfs.C tram_multivertex_bfs.decl.h tram_multivertex_bfs.def.h

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
