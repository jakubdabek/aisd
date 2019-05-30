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
		for k in $$(seq 1 10); do \
			./main --size $$i --write data/$$i/flow-$$k; \
		done; \
	done

test-flow: optimized
	for i in $$(seq 1 16); do \
		for k in $$(seq 1 10); do \
			./main --size $$i --read data/$$i/flow-$$k --terse; \
		done; \
	done > out-flow-$$(date -Iminutes).txt;

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
		done; \
	done > out-match-$$(date -Iminutes).txt;

.PHONY: all clean test
