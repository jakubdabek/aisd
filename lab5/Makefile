CXXFLAGS := -std=c++2a -Wall -Wextra -g
CXX := g++

all: main

optimized: CXXFLAGS+=-O2
optimized: all

debug: CXXFLAGS+=-O0 -g
debug: all

main: main.cpp MaxFlow.o BipartiteMatching.o util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.hpp,$^) $(LDXXFLAGS)

%.o: %.cpp %.hpp util.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $(filter-out %.hpp,$^)

clean:
	$(RM) *.o main

test: test-flow test-match

test-flow-init: optimized
	for i in $$(seq 1 16); do \
		mkdir -p data/$$i; \
		for k in $$(seq 1 40); do \
			./main --size $$i --write data/$$i/flow-$$k; \
		done; \
	done

test-flow: optimized
	for i in $$(seq 1 16); do \
		for k in $$(seq 1 40); do \
			./main --size $$i --read data/$$i/flow-$$k --terse; \
		done; \
		echo Done $$i >&2; \
	done > out/flow.txt;

test-match-init: optimized
	for i in $$(seq 1 16); do \
		for j in $$(seq 1 $$i); do \
			mkdir -p data/$$i-$$j; \
			for k in $$(seq 1 50); do \
				./main --size $$i --degree $$j --write data/$$i-$$j/match-$$k; \
			done; \
		done; \
	done

test-match: optimized
	for i in $$(seq 1 16); do \
		for j in $$(seq 1 $$i); do \
			for k in $$(seq 1 50); do \
				./main --size $$i --degree $$j --read data/$$i-$$j/match-$$k --terse; \
			done; \
			echo Done $$i-$$j >&2; \
		done; \
	done > out/match.txt;

glpk-flow-init: main
	for i in $$(seq 1 16); do \
		mkdir -p data/glpk/$$i; \
		for k in $$(seq 1 40); do \
			./main --size $$i --read data/$$i/flow-$$k --glpk data/glpk/$$i/flow-$$k; \
		done; \
	done

glpk-flow: main
	mkdir -p out/glpk; \
	echo -n > out/glpk-flow.txt; \
	for i in $$(seq 1 16); do \
		for k in $$(seq 1 40); do \
			glpsol --math maxflow.mod --data data/glpk/$$i/flow-$$k --output out/glpk/flow-$$i-$$k.txt > /dev/null; \
			cat out/glpk/flow-$$i-$$k.txt | head -n6 | tail -n1 | cut -d' ' -f5 >> out/glpk-flow.txt; \
		done; \
		echo Done $$i >&2; \
	done

glpk-match-init: main
	for i in $$(seq 1 11); do \
		for j in $$(seq 1 $$i); do \
			mkdir -p data/glpk/$$i-$$j; \
			for k in $$(seq 1 50); do \
				./main --size $$i --degree $$j --read data/$$i-$$j/match-$$k --glpk data/glpk/$$i-$$j/match-$$k; \
			done; \
		done; \
	done

glpk-match: main
	mkdir -p out/glpk; \
	echo -n > out/glpk-match.txt; \
	for i in $$(seq 1 10); do \
		for j in $$(seq 1 $$i); do \
			for k in $$(seq 1 50); do \
				glpsol --math bipartite-matching.mod --data data/glpk/$$i-$$j/match-$$k --output out/glpk/match-$$i-$$j-$$k.txt >/dev/null;  \
				cat out/glpk/match-$$i-$$j-$$k.txt | head -n6 | tail -n1 | cut -d' ' -f5 >> out/glpk-match.txt; \
			done; \
			echo Done $$i-$$j >&2; \
		done; \
	done

.PHONY: all clean test
