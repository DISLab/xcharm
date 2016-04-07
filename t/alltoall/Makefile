-include ../../config.mk
-include ../common.mk
CHARMXI=../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I. -I../../include -I../../include/ucharelib -I../../include/NDMeshStreamer
CHARMC_LINK_FLAGS=-L../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer -module completion -module ampi

OBJS = test_alltoall_tram.o

all: test_alltoall_tram

test_alltoall_tram: test_alltoall_tram.o  
	$(CHARMC) -g -o test_alltoall_tram $^ $(CHARMC_LINK_FLAGS) 

test_alltoall_tram.o: test_alltoall_tram.C cifiles
	$(CHARMC) -g -o test_alltoall_tram.o -c test_alltoall_tram.C  $(CHARMC_COMPILE_FLAGS)

cifiles: test_alltoall_tram.ci
	$(CHARMXI) $<

clean: 
	rm -f *.decl.h *.def.h *.o charmrun test_alltoall_tram

test: all
	@printf "Alltoall (np=2)..."
	@if $(call run, ./test_alltoall_tram +p2 ); \
		then echo ...done; \
		else echo ...failed;\
	fi
