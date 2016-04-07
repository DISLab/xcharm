-include ../common.mk
CHARMC=../../bin/charmc 
CHARMXI=../../bin/charmxi
CHARMC_COMPILE_FLAGS=-I. -I../../include -I../../include/ucharelib -I../../include/NDMeshStreamer
CHARMC_LINK_FLAGS=-L../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer -module completion -module ampi

OBJS = test_proxy.o

all: test_contribute test_proxy test_reentrant_ep

test_contribute: test_contribute.o  
	$(CHARMC) -g -o test_contribute $^ $(CHARMC_LINK_FLAGS) 

test_contribute.o: test_contribute.C test_contribute.decl.h
	$(CHARMC) -g -o test_contribute.o -c test_contribute.C  $(CHARMC_COMPILE_FLAGS)

test_contribute.decl.h: test_contribute.ci
	$(CHARMXI) $<

test_proxy: test_proxy.o  
	$(CHARMC) -g -o test_proxy $^ $(CHARMC_LINK_FLAGS) 

test_proxy.o: test_proxy.C test_proxy.decl.h
	$(CHARMC) -g -o test_proxy.o -c test_proxy.C  $(CHARMC_COMPILE_FLAGS)

test_proxy.decl.h: test_proxy.ci
	$(CHARMXI) $<

test_reentrant_ep: test_reentrant_ep.o  
	$(CHARMC) -g -o test_reentrant_ep $^ $(CHARMC_LINK_FLAGS) 

test_reentrant_ep.o: test_reentrant_ep.C test_reentrant_ep.decl.h
	$(CHARMC) -g -o test_reentrant_ep.o -c test_reentrant_ep.C  $(CHARMC_COMPILE_FLAGS)

test_reentrant_ep.decl.h: test_reentrant_ep.ci
	$(CHARMXI) $<

clean: 
	rm -f *.decl.h *.def.h *.o charmrun 
	rm -f test_contribute test_proxy test_reentrant_ep

test: all
	@printf "test_contribute (np=2)..."
	@if $(call run, ./test_contribute +p2 ); \
		then echo ...done; \
		else echo ...failed;\
	fi
	@printf "test_proxy (np=2)..."
	@if $(call run, ./test_proxy +p2 ); \
		then echo ...done; \
		else echo ...failed;\
	fi
	@printf "test_reentrant_ep (np=2)..."
	@if $(call run, ./test_reentrant_ep +p2 ); \
		then echo ...done; \
		else echo ...failed;\
	fi
