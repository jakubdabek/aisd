CXXFLAGS := -std=c++2a -Wall -Wextra
CXX := g++

all: main

main: main.cpp Graph.o Graph.hpp priority_queue.tpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.tpp,$^) $(LDXXFLAGS)
Graph.o: Graph.cpp Graph.hpp priority_queue.tpp

clean:
	$(RM) *.o main

.PHONY: all clean