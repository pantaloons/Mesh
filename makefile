CC = gcc
CFLAGS = -Wall -g -Wextra -std=c99 -pedantic
LDFLAGS = -lm -lglut -lGLU -lGL

all: display

display: display.o mesh.o meshio.o heap.o
	$(CC) $(LDFLAGS) -o $@ $^
	
display.o: display.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
heap.o: heap.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
meshio.o: meshio.c
	$(CC) $(CFLAGS) -Wno-unused-result -c -o $@ $<

mesh.o: mesh.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f raptor raptor.o meshio.o mesh.o heap.o core.* vgcore.*
	
.PHONY: clean
