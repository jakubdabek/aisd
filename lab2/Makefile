CC:=g++
CXXFLAGS:=-Wall -pedantic -std=c++17

all: main

optimized: CXXFLAGS+=-O2
optimized: all

main: main.o

main.o: main.cpp heap.tpp util.tpp sorting.tpp

clean:
	$(RM) main main.o

.PHONY: all clean