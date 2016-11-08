include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS=  -g -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS) -Wno-deprecated-declarations
CHARMC_LINK_FLAGS= -L./.tmp -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

RMAT_BINARY= charm_bfs-rmat tram_bfs_radix-rmat charm_multivertex_bfs-rmat tram_multivertex_bfs-rmat 
SSCA_BINARY= charm_bfs-ssca tram_bfs_radix-ssca charm_multivertex_bfs-ssca tram_multivertex_bfs-ssca 
RANDOM_BINARY= charm_bfs-random tram_bfs_radix-random charm_multivertex_bfs-random tram_multivertex_bfs-random 

rmat: $(RMAT_BINARY) 

ssca: $(SSCA_BINARY) 

random: $(RANDOM_BINARY)

all: dir rmat ssca random

charm_bfs-rmat: charm_bfs-rmat.o
charm_bfs-rmat.o: charm_bfs.C charm_bfs.decl.h charm_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_bfs_radix-rmat: tram_bfs_radix-rmat.o
tram_bfs_radix-rmat.o: tram_bfs_radix.C tram_bfs_radix.decl.h tram_bfs_radix.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_multivertex_bfs-rmat: charm_multivertex_bfs-rmat.o
charm_multivertex_bfs-rmat.o: charm_multivertex_bfs.C charm_multivertex_bfs.decl.h charm_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_multivertex_bfs-rmat: tram_multivertex_bfs-rmat.o
tram_multivertex_bfs-rmat.o: tram_multivertex_bfs.C tram_multivertex_bfs.decl.h tram_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_bfs-ssca: charm_bfs-ssca.o
charm_bfs-ssca.o: charm_bfs.C charm_bfs.decl.h charm_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

tram_bfs_radix-ssca: tram_bfs_radix-ssca.o
tram_bfs_radix-ssca.o: tram_bfs_radix.C tram_bfs_radix.decl.h tram_bfs_radix.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

charm_multivertex_bfs-ssca: charm_multivertex_bfs-ssca.o
charm_multivertex_bfs-ssca.o: charm_multivertex_bfs.C charm_multivertex_bfs.decl.h charm_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

tram_multivertex_bfs-ssca: tram_multivertex_bfs-ssca.o
tram_multivertex_bfs-ssca.o: tram_multivertex_bfs.C tram_multivertex_bfs.decl.h tram_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

charm_bfs-random: charm_bfs-random.o
charm_bfs-random.o: charm_bfs.C charm_bfs.decl.h charm_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

tram_bfs_radix-random: tram_bfs_radix-random.o
tram_bfs_radix-random.o: tram_bfs_radix.C tram_bfs_radix.decl.h tram_bfs_radix.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

charm_multivertex_bfs-random: charm_multivertex_bfs-random.o
charm_multivertex_bfs-random.o: charm_multivertex_bfs.C charm_multivertex_bfs.decl.h charm_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

tram_multivertex_bfs-random: tram_multivertex_bfs-random.o
tram_multivertex_bfs-random.o: tram_multivertex_bfs.C tram_multivertex_bfs.decl.h tram_multivertex_bfs.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

%: %.o 
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ .tmp/$< -module NDMeshStreamer

%.o: %.C %.decl.h %.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

%.xi : %.ci
	gcc -x c++ -E -P $< > $@

%.def.h %.decl.h: %.xi
	$(CHARMXI) $<
	sed -i 's/CkIndex_GraphLib__/GraphLib::CkIndex_/g' $(basename $<).decl.h $(basename $<).def.h
	sed -i 's/GraphLib__/GraphLib::/g' $(basename $<).decl.h $(basename $<).def.h
	mv *.xi .tmp && mv *.decl.h .tmp && mv *.def.h .tmp

clean: 
	rm -f $(RMAT_BINARY) $(SSCA_BINARY) $(RANDOM_BINARY)
	rm -f .tmp/*.xi .tmp/*.decl.h .tmp/*.def.h .tmp/*.o charmrun 

clean-all:
	rm -rf .tmp

.PHONY: dir

dir:
	mkdir -p .tmp
