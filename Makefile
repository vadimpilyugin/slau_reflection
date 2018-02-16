CFLAGS=-Wall

all: mkdir build/main build/tests

mkdir: 
	mkdir -p build

# Object files
build/matrix.o: matrix.c matrix.h
	gcc -o $@ -c matrix.c $(CFLAGS)

build/main.o: main.c matrix.h
	gcc -o $@ -c main.c $(CFLAGS)

build/tests.o: tests.c matrix.h 
	gcc -o $@ -c tests.c $(CFLAGS)

# Executable
build/main: build/main.o build/matrix.o
	gcc -o $@ build/main.o build/matrix.o -lm

# Tests
build/tests: build/tests.o build/matrix.o
	gcc -o $@ build/tests.o build/matrix.o -lm

run: build/main
	# ======================
	build/main

test: build/tests
	# ======================
	build/tests

clean:
	rm -rf build/*
