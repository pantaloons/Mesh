#ifndef __MESH_H__
#define __MESH_H__

#include <stdlib.h>
#include <math.h>

typedef struct _edge {
	int index;
	struct _vertex *vert;
	struct _face *face;
	struct _edge *prev, *next, *pair;
} Edge;

typedef struct _vertex {
	int index;
	float x, y, z;
	struct _edge *edge;
} Vertex;

typedef struct _face {
	int index;
	struct _edge *edge;
} Face;

typedef struct _mesh {
	int numEdges, numVertices, numFaces;
	struct _edge **edges;
	struct _face **faces;
	struct _vertex **verts;
} Mesh;

Mesh* initMesh(int numVertices, int numFaces, int numEdges, Vertex **verts, Face **faces, Edge **edges);
void destroyMesh(Mesh *m);

void faceNormal(Face *f, float result[3]);

void deleteVert(Mesh *m, Vertex *v);
void deleteEdge(Mesh *m, Edge *e);
void deleteFace(Mesh *m, Face *f);

void localDelaunay(Mesh *m, Edge *e);

float simpleCost(Edge *e);
float garlandCost(Edge *e);

int collapsable(Edge *e);
void collapseEdge(Mesh *m, Edge *edge);

#endif
