CXXFLAGS := -std=c++2a -Wall -Wextra
CXX := g++

all: main

main: main.cpp DirectedGraph.o DirectedGraph.hpp RealUndirectedGraph.o RealUndirectedGraph.hpp priority_queue.tpp util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.tpp, $(filter-out %.hpp,$^)) $(LDXXFLAGS)
DirectedGraph.o: DirectedGraph.cpp DirectedGraph.hpp priority_queue.tpp util.hpp

RealUndirectedGraph.o: RealUndirectedGraph.cpp RealUndirectedGraph.hpp util.hpp

clean:
	$(RM) *.o main

.PHONY: all clean