include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS) -Wno-deprecated-declarations
CHARMC_LINK_FLAGS=-L./.tmp -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_parallel_search charm_parallel_search_radix \
				 charm_multivertex_parallel_search \
				 tram_parallel_search tram_parallel_search_radix \
				 tram_multivertex_parallel_search
#				 ucharelib_parallel_search ucharelib_parallel_search_radix

all: dir $(BINARY)

charm_parallel_search: charm_parallel_search.o
charm_parallel_search.o: charm_parallel_search.C charm_parallel_search.decl.h charm_parallel_search.def.h

charm_parallel_search_radix: charm_parallel_search_radix.o
charm_parallel_search_radix.o: charm_parallel_search_radix.C charm_parallel_search_radix.decl.h charm_parallel_search_radix.def.h

tram_parallel_search: tram_parallel_search.o
tram_parallel_search.o: tram_parallel_search.C tram_parallel_search.decl.h tram_parallel_search.def.h

tram_parallel_search_radix: tram_parallel_search_radix.o
tram_parallel_search_radix.o: tram_parallel_search_radix.C tram_parallel_search_radix.decl.h tram_parallel_search_radix.def.h

charm_multivertex_parallel_search: charm_multivertex_parallel_search.o
charm_multivertex_parallel_search.o: charm_multivertex_parallel_search.C charm_multivertex_parallel_search.decl.h charm_multivertex_parallel_search.def.h

tram_multivertex_parallel_search: tram_multivertex_parallel_search.o
tram_multivertex_parallel_search.o: tram_multivertex_parallel_search.C tram_multivertex_parallel_search.decl.h tram_multivertex_parallel_search.def.h

ucharelib_parallel_search: ucharelib_parallel_search.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ ./.tmp/$+  
ucharelib_parallel_search.o: ucharelib_parallel_search.C ucharelib_parallel_search.decl.h ucharelib_parallel_search.def.h
	$(CHARMC) $(XCHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

ucharelib_parallel_search_radix: ucharelib_parallel_search_radix.o 
	$(CHARMC) $(XCHARMC_LINK_FLAGS) -language charm++ -o $@ ./.tmp/$+  
ucharelib_parallel_search_radix.o: ucharelib_parallel_search_radix.C ucharelib_parallel_search_radix.decl.h ucharelib_parallel_search_radix.def.h
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
