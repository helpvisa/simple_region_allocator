.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all test clean

SHELL = /bin/sh

CC = cc
CFLAGS = -Wall

all: build

test: build
	./arenas

build: main.o
	$(CC) $(CFLAGS) -o ./arenas ./main.o

main.o: main.c region.h
	$(CC) $(CLFAGS) -c ./main.c -o ./main.o

clean:
	rm -rf ./*.o
	rm -rf ./arenas
