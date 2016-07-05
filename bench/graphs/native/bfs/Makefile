include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS)
CHARMC_LINK_FLAGS=-L./.tmp -L../../../../lib -lGraphGenerator \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_bfs_async charm_bfs_sync charm_bfs_async_radix charm_bfs_sync_radix \
				 charm_multivertex_bfs_async charm_multivertex_bfs_sync \
				 tram_bfs_async tram_bfs_sync tram_bfs_async_radix tram_bfs_sync_radix \
				 tram_multivertex_bfs_async
#				 ucharelib_bfs_async ucharelib_bfs_async_radix

all: dir $(BINARY)

charm_bfs_async: charm_bfs_async.o
charm_bfs_async.o: charm_bfs_async.C charm_bfs_async.decl.h charm_bfs_async.def.h

charm_bfs_sync: charm_bfs_sync.o
charm_bfs_sync.o: charm_bfs_sync.C charm_bfs_sync.decl.h charm_bfs_sync.def.h

charm_bfs_async_radix: charm_bfs_async_radix.o
charm_bfs_async_radix.o: charm_bfs_async_radix.C charm_bfs_async_radix.decl.h charm_bfs_async_radix.def.h

charm_bfs_sync_radix: charm_bfs_sync_radix.o
charm_bfs_sync_radix.o: charm_bfs_sync_radix.C charm_bfs_sync_radix.decl.h charm_bfs_sync_radix.def.h

tram_bfs_async: tram_bfs_async.o
tram_bfs_async.o: tram_bfs_async.C tram_bfs_async.decl.h tram_bfs_async.def.h

tram_bfs_sync: tram_bfs_sync.o
tram_bfs_sync.o: tram_bfs_sync.C tram_bfs_sync.decl.h tram_bfs_sync.def.h

tram_bfs_async_radix: tram_bfs_async_radix.o
tram_bfs_async_radix.o: tram_bfs_async_radix.C tram_bfs_async_radix.decl.h tram_bfs_async_radix.def.h

tram_bfs_sync_radix: tram_bfs_sync_radix.o
tram_bfs_sync_radix.o: tram_bfs_sync_radix.C tram_bfs_sync_radix.decl.h tram_bfs_sync_radix.def.h

charm_multivertex_bfs_async: charm_multivertex_bfs_async.o
charm_multivertex_bfs_async.o: charm_multivertex_bfs_async.C charm_multivertex_bfs_async.decl.h charm_multivertex_bfs_async.def.h

charm_multivertex_bfs_sync: charm_multivertex_bfs_sync.o
charm_multivertex_bfs_sync.o: charm_multivertex_bfs_sync.C charm_multivertex_bfs_sync.decl.h charm_multivertex_bfs_sync.def.h

tram_multivertex_bfs_async: tram_multivertex_bfs_async.o
tram_multivertex_bfs_async.o: tram_multivertex_bfs_async.C tram_multivertex_bfs_async.decl.h tram_multivertex_bfs_async.def.h

ucharelib_bfs_async: ucharelib_bfs_async.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ ./.tmp/$+  
ucharelib_bfs_async.o: ucharelib_bfs_async.C ucharelib_bfs_async.decl.h ucharelib_bfs_async.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

ucharelib_bfs_async_radix: ucharelib_bfs_async_radix.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ ./.tmp/$+  
ucharelib_bfs_async_radix.o: ucharelib_bfs_async_radix.C ucharelib_bfs_async_radix.decl.h ucharelib_bfs_async_radix.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

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
