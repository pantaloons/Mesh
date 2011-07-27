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

HashMap* initMap(int capacity);
int edgeEqual(EdgeID id1, EdgeID id2);
void mapPut(HashMap *map, EdgeID key, Edge *value);
Edge* mapGet(HashMap* map, EdgeID key);
Mesh* readMesh(char* fileName);

#endif
