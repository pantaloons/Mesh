CC = gcc
CFLAGS = -Wall -g -Wextra -std=c99 -pedantic -O4
LDFLAGS =  -lm -lglut -lGLU -lGL

all: reduce

reduce: reduce.o mesh.o meshio.o heap.o
	$(CC) $(LDFLAGS) -o $@ $^
	
reduce.o: reduce.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
heap.o: heap.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
meshio.o: meshio.c
	$(CC) $(CFLAGS) -Wno-unused-result -c -o $@ $<

mesh.o: mesh.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f reduce *.o vgcore.*
	
.PHONY: clean
