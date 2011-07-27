#include "mesh.h"

Mesh* initMesh(int numVertices, int numFaces, int numEdges, Vertex** verts, Face** faces, Edge** edges) {
	Mesh* m = malloc(sizeof(Mesh));
	m->numVertices = numVertices;
	m->numFaces = numFaces;
	m->numEdges = numEdges;
	m->verts = verts;
	m->faces = faces;
	m->edges = edges;
	return m;
}

void destroyMesh(Mesh* m) {
	int i;
	for(i = 0; i < m->numEdges; i++) free(m->edges[i]);
	for(i = 0; i < m->numVertices; i++) free(m->verts[i]);
	for(i = 0; i < m->numFaces; i++) free(m->faces[i]);
	free(m->edges);
	free(m->verts);
	free(m->faces);
	free(m);
}
