CHARMC=$(CHARM_HOME)/bin/charmc -std=c++11 $(OPTS)
GRAPHLIB=..

TARGETS= test

all: $(TARGETS)

#--------------------------------------------------------------------
test: test.o $(GRAPHLIB)/libgraphio.a 
	$(CHARMC) -language charm++ -o test $< $(GRAPHLIB)/libgraphio.a 

test.o: test.C test.decl.h $(GRAPHLIB)/InputOutput.h
	$(CHARMC) -c test.C -I $(GRAPHLIB)

test.decl.h: test.ci
	$(CHARMC)  test.ci
#--------------------------------------------------------------------

clean:
	rm -f *.decl.h *.def.h conv-host *.o *.a charmrun *.log *.sum *.sts 
	rm -f $(TARGETS)
