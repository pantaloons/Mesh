#include "mesh.h"

Mesh *initMesh(int numVertices, int numFaces, int numEdges, Vertex** verts, Face** faces, Edge** edges) {
	Mesh *m = malloc(sizeof(Mesh));
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

/**
* Deletes the vertex out of the mesh. This does NOT "remove" it
* from said mesh and will leave dangling references if called wrongly.
* The vertex should be removed from the mesh before being deleted.
*/
void deleteVert(Mesh *m, Vertex *v) {
	m->verts[v->index] = m->verts[m->numVertices - 1];
	m->verts[v->index]->index = v->index;
	m->numVertices -= 1;
	free(v);
}

/**
* Deletes the edge out of the mesh. This does NOT "remove" it
* from said mesh and will leave dangling references if called wrongly.
* The edge should be removed from the mesh before being deleted.
*/
void deleteEdge(Mesh *m, Edge *e) {
	m->edges[e->index] = m->edges[m->numEdges - 1];
	m->edges[e->index]->index = e->index;
	m->numEdges -= 1;
	free(e);
}

/**
* Deletes the face out of the mesh. This does NOT "remove" it
* from said mesh and will leave dangling references if called wrongly.
* The face should be removed from the mesh before being deleted.
*/
void deleteFace(Mesh *m, Face *f) {
	m->faces[f->index] = m->faces[m->numFaces - 1];
	m->faces[f->index]->index = f->index;
	m->numFaces -= 1;
	free(f);
}

/**
* Determine if v is a boundary vertex.
*/
int boundaryVertex(Vertex *v) {
	Edge *e = v->edge;
	do {
		if(e->pair == NULL) return 1;
		e = e->pair->prev;
	}
	while(e != v->edge);
	return 0;
}

/**
* Determine if the edge e is collapsable without causing topology errors
*/
int collapsable(Edge *e) {
	Edge *ring1, *ring2;
	Vertex *a, *b;
	/* Case (a), edge belongs to a triangle, where the other two edges are boundary edges 
	   This shouldn't happen for manifolds */
	if(e->next->pair == NULL && e->prev->pair == NULL) return 0;
	if(e->pair != NULL && e->pair->next->pair == NULL && e->pair->prev->pair == NULL) return 0;
	
	/* Case (b), incident vertices are boundary vertices but edge is not a boundary edge
		This shouldn't happen for manifolds */
	if(e->pair != NULL && boundaryVertex(e->vert) && boundaryVertex(e->pair->vert)) return 0;
	
	/* Case (c), the intersection of the one ring neighbourhoods of the incident vertices
	   contains more than just the two incident vertices */	
	a = e->next->vert;
	b = e->pair->next->vert;
	ring1 = e;
	do {
		ring2 = e->pair;
		do {			
			Vertex *v1 = ring1->pair->vert;
			Vertex *v2 = ring2->pair->vert;
			if(v1 == v2 && v1 != a && v1 != b) return 0;
			
			ring2 = ring2->pair->prev;
		}
		while(ring2 != e->pair);
		ring1 = ring1->pair->prev;
	}
	while(ring1 != e);
	
	return 1;
}

void collapseEdge(Mesh *m, Edge *e) {
	Edge *edge;
	Vertex *p;
	
	Edge *a = e->prev;
	Edge *c = e->pair->next;
	
	Edge *b1 = e->next;
	Edge *b2 = b1->pair;
	Edge *d1 = e->pair->prev;
	Edge *d2 = d1->pair;
	
	p = e->pair->vert;
	/* Re link vertex of edges incident at Q to P */
	edge = d1;
	do {
		edge = edge->pair->prev;
		edge->vert = p;
	} while(edge != b2);
	
	/* Re link left side traingle */
	a->prev = b2->prev;
	b2->prev->next = a;
	a->next = b2->next;
	b2->next->prev = a;
	a->face = b2->face;
	if(b2->face->edge == b2) b2->face->edge = a;
	if(b1->vert->edge == b1) b1->vert->edge = a->pair;
	
	/* Re link right side triangle */
	c->prev = d2->prev;
	d2->prev->next = c;
	c->next = d2->next;
	d2->next->prev = c;
	c->face = d2->face;
	if(d2->face->edge == d2) d2->face->edge = c;
	if(c->vert->edge == d2) c->vert->edge = c;
	
	/* Re link edge referred to by P */
	p = e->pair->vert;
	if(p->edge == e->pair) p->edge = a;
	
	
	//p->x = (p->x + e->vert->x)/2.0f;
	//p->y = (p->y + e->vert->y)/2.0f;
	//p->z = (p->z + e->vert->z)/2.0f;
	
	deleteEdge(m, b1);
	deleteEdge(m, d1);
	deleteEdge(m, b2);
	deleteEdge(m, d2);
	deleteFace(m, e->face);
	deleteFace(m, e->pair->face);
	printf("Deleting vert %d\n", e->vert->index);
	deleteVert(m, e->vert);
	deleteEdge(m, e->pair);
	deleteEdge(m, e);
}
