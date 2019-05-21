CXXFLAGS := -std=c++2a -Wall -Wextra
CXX := g++
TYPE=std::string

all: main

optimized: CXXFLAGS+=-O2
optimized: all

debug: CXXFLAGS+=-O0 -g
debug: TYPE:=int
debug: all

debug-string: CXXFLAGS+=-O0 -g
debug-string: all

main: main.cpp BinarySearchTree.hpp RedBlackTree.hpp SplayTree.hpp TreeUtil.hpp util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.tpp, $(filter-out %.hpp,$^)) $(LDXXFLAGS) -DTYPE=$(TYPE)

clean:
	$(RM) *.o main

test:
	py test.py

.PHONY: all clean test
