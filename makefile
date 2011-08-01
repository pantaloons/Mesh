CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -pedantic
LDFLAGS = -lm -lglut -lGLU -lGL

all: raptor

raptor: raptor.o mesh.o meshio.o heap.o
	$(CC) $(LDFLAGS) -o $@ $^
	
raptor.o: raptor.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
heap.o: heap.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
meshio.o: meshio.c
	$(CC) $(CFLAGS) -c -o $@ $<

mesh.o: mesh.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f raptor raptor.o meshio.o mesh.o heap.o core.* vgcore.*
	
.PHONY: clean
