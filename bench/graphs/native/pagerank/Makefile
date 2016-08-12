include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -O3 -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include -Wno-deprecated-declarations
CHARMC_LINK_FLAGS=-g -L./.tmp  -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono \
									-module ampi

XCHARMC_COMPILE_FLAGS=-O3 -I. -I../../../../include -I../../../../include/ucharelib -I../../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-O3 -L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_pagerank_naive charm_pagerank_intracount charm_pagerank_async \
				 tram_pagerank_naive 
#ucharelib_pagerank ucharelib_pagerank_radius

all: dir $(BINARY)

charm_pagerank_naive: charm_pagerank_naive.o
charm_pagerank_naive.o: charm_pagerank_naive.C charm_pagerank_naive.decl.h charm_pagerank_naive.def.h

charm_pagerank_intracount: charm_pagerank_intracount.o
charm_pagerank_intracount.o: charm_pagerank_intracount.C charm_pagerank_intracount.decl.h charm_pagerank_intracount.def.h

charm_pagerank_async: charm_pagerank_async.o
charm_pagerank_async.o: charm_pagerank_async.C charm_pagerank_async.decl.h charm_pagerank_async.def.h

tram_pagerank_naive: tram_pagerank_naive.o
tram_pagerank_naive.o: tram_pagerank_naive.C tram_pagerank_naive.decl.h tram_pagerank_naive.def.h

ucharelib_pagerank: ucharelib_pagerank.o 
ucharelib_pagerank.o: ucharelib_pagerank.C ucharelib_pagerank.decl.h ucharelib_pagerank.def.h

ucharelib_pagerank_radius: ucharelib_pagerank_radius.o 
ucharelib_pagerank_radius.o: ucharelib_pagerank_radius.C ucharelib_pagerank_radius.decl.h ucharelib_pagerank_radius.def.h

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
