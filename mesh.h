#ifndef __MESH_H__
#define __MESH_H__

#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "heap.h"

Mesh* initMesh(int numVertices, int numFaces, int numEdges, Vertex **verts, Face **faces, Edge **edges);
void destroyMesh(Mesh *m);

void faceNormal(Face *f, float result[3]);

void deleteVert(Mesh *m, Vertex *v);
void deleteEdge(Mesh *m, Edge *e);
void deleteFace(Mesh *m, Face *f);

void edgeFlip(Edge *e);
void localDelaunay(Vertex *e);
void recalculate(Mesh *m, Vertex *v);

float simpleCost(Edge *e);
float melaxCost(Edge *e);
float garlandCost(Edge *e);

int collapsable(Edge *e);
void reduce(Mesh *m);
Vertex *collapseEdge(Mesh *m, Edge *e);

#endif
