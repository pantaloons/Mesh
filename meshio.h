#ifndef __MESHIO_H__
#define __MESHIO_H__

#include <stdio.h>
#include <string.h>
#include "mesh.h"

typedef struct _edgeid {
	int v1, v2;
} EdgeID;

typedef struct _mapnode {
	struct _edgeid key;
	Edge *value;
	
	struct _mapnode *next;
} MapNode;

typedef struct _hashmap {
	int modulus;
	struct _mapnode **map;
} HashMap;

Mesh *readMesh(char *fileName);
void printMesh(Mesh *m, FILE *f);

#endif
