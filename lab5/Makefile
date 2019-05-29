CXXFLAGS := -std=c++2a -Wall -Wextra -g
CXX := g++

all: main

optimized: CXXFLAGS+=-O2
optimized: all

debug: CXXFLAGS+=-O0 -g
debug: all

main: main.cpp util.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.hpp,$^) $(LDXXFLAGS)

clean:
	$(RM) *.o main

test:
	for i in $$(seq 1 16); do \
		for j in $$(seq 1 5); do \
			./main --size $$i; \
		done; \
	done;

.PHONY: all clean test
