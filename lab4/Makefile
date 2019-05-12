CXXFLAGS := -std=c++2a -Wall -Wextra -O2
CXX := g++

all: main

main: main.cpp BinarySearchTree.hpp TreeUtil.hpp util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.tpp, $(filter-out %.hpp,$^)) $(LDXXFLAGS)

clean:
	$(RM) *.o main

.PHONY: all clean