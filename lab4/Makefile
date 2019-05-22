CXXFLAGS := -std=c++2a -Wall -Wextra -g
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
	for i in $$(seq 1 2); do \
		py test.py __$$i; \
	done;

.PHONY: all clean test
