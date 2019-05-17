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
	./main --type rbt < data/operations1.txt > out1-rbt.txt 2>err1-rbt.txt
	./main --type rbt < data/operations2.txt > out2-rbt.txt 2>err2-rbt.txt
	./main --type rbt < data/operations3.txt > out3-rbt.txt 2>err3-rbt.txt
	./main --type rbt < data/operations4.txt > out4-rbt.txt 2>err4-rbt.txt
	./main --type splay < data/operations1.txt > out1-splay.txt 2>err1-splay.txt
	./main --type splay < data/operations2.txt > out2-splay.txt 2>err2-splay.txt
	./main --type splay < data/operations3.txt > out3-splay.txt 2>err3-splay.txt
	./main --type splay < data/operations4.txt > out4-splay.txt 2>err4-splay.txt
	./main --type bst < data/operations1.txt > out1-bst.txt 2>err1-bst.txt
	#./main --type bst < data/operations2.txt > out2-bst.txt 2>err2-bst.txt

.PHONY: all clean test
