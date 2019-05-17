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
	for i in sample lotr wordlist KJB; do\
		./main --type rbt < data/operations_$$i.txt > io/out_$$i-rbt.txt 2> io/err_$$i-rbt.txt;\
		./main --type splay < data/operations_$$i.txt > io/out_$$i-splay.txt 2> io/err_$$i-splay.txt;\
		if [ $$i != wordlist ] ; then\
			./main --type bst < data/operations_$$i.txt > io/out_$$i-bst.txt 2> io/err_$$i-bst.txt;\
		fi;\
	done;
	for i in $$(seq 0 4); do\
		./main --type rbt < data/operations_permuted$$i.txt > io/out_permuted$$i-rbt.txt 2> io/err_permuted$$i-rbt.txt;\
		./main --type splay < data/operations_permuted$$i.txt > io/out_permuted$$i-splay.txt 2> io/err_permuted$$i-splay.txt;\
		./main --type bst < data/operations_permuted$$i.txt > io/out_permuted$$i-bst.txt 2> io/err_permuted$$i-bst.txt;\
	done;

.PHONY: all clean test
