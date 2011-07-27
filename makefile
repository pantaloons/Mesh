CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -pedantic
LDFLAGS = -lglut -lGLU -lGL --stack=67108864

all: raptor

raptor: raptor.o mesh.o meshio.o
	$(CC) $(LDFLAGS) -o $@ $^
	
raptor.o: raptor.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
meshio.o: meshio.c
	$(CC) $(CFLAGS) -c -o $@ $<

mesh.o: mesh.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f raptor raptor.o meshio.o mesh.o core.* vgcore.*
	
.PHONY: clean
