-include ../common.mk
CHARMC=../../bin/charmc 
CHARMXI=../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I. -I../../include -I../../include/ucharelib -I../../include/NDMeshStreamer
CHARMC_LINK_FLAGS=-L../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer -module completion -module ampi

OBJS = test_hello_tram.o

all: test_hello_tram

test_hello_tram: test_hello_tram.o  
	$(CHARMC) -g -o test_hello_tram $^ $(CHARMC_LINK_FLAGS) 

test_hello_tram.o: test_hello_tram.C cifiles
	$(CHARMC) -g -o test_hello_tram.o -c test_hello_tram.C  $(CHARMC_COMPILE_FLAGS)

cifiles: test_hello_tram.ci
	$(CHARMXI) $<

clean: 
	rm -f *.decl.h *.def.h *.o charmrun test_hello_tram

test: all
	@printf "Hello (np=2)..."
	@if $(call run, ./test_hello_tram +p2 ); \
		then echo ...done; \
		else echo ...failed;\
	fi

