#ifndef __MESH_H__
#define __MESH_H__

#include <stdlib.h>

typedef struct _edge {
	struct _vertex *vert;
	struct _face *face;
	struct _edge *prev, *next, *pair;
} Edge;

typedef struct _vertex {
	float x, y, z;
	struct _edge *edge;
} Vertex;

typedef struct _face {
	struct _edge *edge;
} Face;

typedef struct _mesh {
	int numEdges, numVertices, numFaces;
	struct _edge **edges;
	struct _face **faces;
	struct _vertex **verts;
} Mesh;

Mesh* initMesh(int numVertices, int numFaces, int numEdges, Vertex** verts, Face** faces, Edge** edges);
void destroyMesh(Mesh* m);

#endif
