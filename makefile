CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -pedantic -O3
LDFLAGS = -lglut -lGLU -lGL

all: mesh

mesh: mesh.o
	$(CC) $(LDFLAGS) -o $@ $^
	
mesh.o: mesh.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f mesh mesh.o
	
.PHONY: clean
