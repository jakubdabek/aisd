CC:=gcc
CFLAGS:=-Wall -pedantic -std=c11
LDLIBS:=-lm

all: main

main: main.o forward_list.o

main.o: main.c forward_list.h

forward_list.o: forward_list.c forward_list.h

clean:
	$(RM) main main.o forward_list.o

.PHONY: all clean