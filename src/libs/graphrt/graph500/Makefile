BOOST_INCLUDE=/home/frolo/local/boost_1_54_0/include
CXX = g++-4.8 -I$(BOOST_INCLUDE)

all: libgraph500.a

libgraph500.a: graph_generator.o mrg_transitions.o
	ar q $@ $<

%.cpp: %.o
	$(CXX) -c $< -o $@

clean:
	rm *.o libgraph500.a
