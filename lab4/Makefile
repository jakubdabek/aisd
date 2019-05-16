CXXFLAGS := -std=c++2a -Wall -Wextra
CXX := g++

all: main

optimized: CXXFLAGS+=-O2
optimized: all

debug: CXXFLAGS+=-O0 -g
debug:all

main: main.cpp BinarySearchTree.hpp RedBlackTree.hpp SplayTree.hpp TreeUtil.hpp util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.tpp, $(filter-out %.hpp,$^)) $(LDXXFLAGS)

clean:
	$(RM) *.o main

.PHONY: all clean