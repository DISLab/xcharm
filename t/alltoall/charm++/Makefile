CHARMC=$(CHARM_HOME)/bin/charmc -g -I.

all: alltoall


alltoall: alltoall.decl.h alltoall.o 
	$(CHARMC) -o $@ alltoall.o -module ampi

%.o:%.C
	$(CHARMC) -c $< -o $@

%.decl.h:%.ci
	$(CHARMC) $< 

clean:
	rm -f alltoall *.o *.decl.h *.def.h 


