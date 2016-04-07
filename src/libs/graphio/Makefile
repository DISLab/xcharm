CHARMC=$(CHARM_HOME)/bin/charmc -std=c++11 -g $(OPTS) -I./GraphHPC-0.2

TARGETS= libgraphio.a

all: $(TARGETS)

#--------------------------------------------------------------------
#libgraphio.a: InputOutput.o GraphHPC-1.0/graph_tools.o 
libgraphio.a: InputOutput.o GraphHPC-0.2/graph_tools.o 
	ar r $@ $^
	cp $@ ../../../lib/
	ln -s ../src/libs/graphio ../../../include/graphio

InputOutput.o: InputOutput.C InputOutput.h InputOutput.decl.h InputOutput.def.h GraphFileIO.h GraphDistr.h GraphGenerator.h graphio.h GraphIO.h
	$(CHARMC) -c $<

InputOutput.h: InputOutput.decl.h 

InputOutput.def.h: InputOutput.decl.h

InputOutput.decl.h: InputOutput.ci
	$(CHARMC) InputOutput.ci
#--------------------------------------------------------------------

clean:
	rm -f *.decl.h *.def.h conv-host *.o *.a charmrun *.log *.sum *.sts 
	rm -f $(TARGETS)
