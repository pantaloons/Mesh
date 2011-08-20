#include "mesh.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float (*currentCost)(Edge*) = simpleCost;

Mesh *initMesh(int numVertices, int numFaces, int numEdges, Vertex** verts, Face** faces, Edge** edges) {
	Mesh *m = (Mesh*)malloc(sizeof(Mesh));
	m->numVertices = numVertices;
	m->numFaces = numFaces;
	m->numEdges = numEdges;
	m->verts = verts;
	m->faces = faces;
	m->edges = edges;
	m->heap = initHeap(m, currentCost, collapsable);
	return m;
}

void changeCostFunc(Mesh *m, float (*func)(Edge*)) {
	m->heap->func = func;
	currentCost = func;
	for(int i = 0; i < m->numEdges; i++) {
		recalculateKey(m->heap, m->edges[i]);
	}
}

void destroyMesh(Mesh* m) {
	int i;
	destroyHeap(m->heap);
	for(i = 0; i < m->numEdges; i++) free(m->edges[i]);
	for(i = 0; i < m->numVertices; i++) free(m->verts[i]);
	for(i = 0; i < m->numFaces; i++) free(m->faces[i]);
	free(m->edges);
	free(m->verts);
	free(m->faces);
	free(m);
}

void faceNormal(Face *f, float result[3]) {
	Edge *edge = f->edge;
	
	Vertex *vert = edge->vert;
	Vertex *v2 = edge->next->vert;
	Vertex *v3 = edge->prev->vert;
	
	float dx1 = (v2->x - vert->x), dx2 = (v3->x - vert->x);
	float dy1 = (v2->y - vert->y), dy2 = (v3->y - vert->y);
	float dz1 = (v2->z - vert->z), dz2 = (v3->z - vert->z);
	
	float cx = dy1*dz2 - dz1*dy2;
	float cy = dz1*dx2 - dx1*dz2;
	float cz = dx1*dy2 - dy1*dx2;
	
	float len = sqrt(cx * cx + cy * cy + cz * cz);
	
	result[0] = cx/len;
	result[1] = cy/len;
	result[2] = cz/len;
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
	removeEdge(m->heap, e);
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

void edgeFlip(Edge *e) {
	Edge *ep = e->prev;
	Edge *en = e->next;
	Edge *epp = e->pair->prev;
	Edge *epn = e->pair->next;
	
	/* Change vert -> edge pointers */
	e->vert->edge = epp;
	e->pair->vert->edge = ep;
	
	/* Change edge -> edge pointers */
	en->prev = epp;
	en->next = e;
	ep->next  = epn;
	ep->prev = e->pair;
	
	epn->prev = ep;
	epn->next = e->pair;
	epp->next = en;
	epp->prev = e;
	
	e->prev = en;
	e->next = epp;
	e->pair->prev = epn;
	e->pair->next = ep;

	/* Change edge -> vert pointers */
	e->vert = epn->vert;
	e->pair->vert = en->vert;
	
	/* Change edge -> face pointers */
	epp->face = e->face;
	ep->face = e->pair->face;
	
	/* Change face -> edge pointers */
	e->face->edge = e;
	e->pair->face->edge = e->pair;
}

double magnitude(Edge *e) {
	double dx = e->vert->x - e->pair->vert->x;
	double dy = e->vert->y - e->pair->vert->y;
	double dz = e->vert->z - e->pair->vert->z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

double minAngle(Edge *e1) {
	Edge *e2 = e1->next;
	Edge *e3 = e2->next;
	
	double len1 = sqrt((e1->vert->x - e3->vert->x) * (e1->vert->x - e3->vert->x) +
				  (e1->vert->y - e3->vert->y) * (e1->vert->y - e3->vert->y) +
				  (e1->vert->z - e3->vert->z) * (e1->vert->z - e3->vert->z));
	double len2 = sqrt((e2->vert->x - e3->vert->x) * (e2->vert->x - e3->vert->x) +
				  (e2->vert->y - e3->vert->y) * (e2->vert->y - e3->vert->y) +
				  (e2->vert->z - e3->vert->z) * (e2->vert->z - e3->vert->z));
	double len3 = sqrt((e2->vert->x - e1->vert->x) * (e2->vert->x - e1->vert->x) +
				  (e2->vert->y - e1->vert->y) * (e2->vert->y - e1->vert->y) +
				  (e2->vert->z - e1->vert->z) * (e2->vert->z - e1->vert->z));
				  
	double t1 = acos(((e1->vert->x - e3->vert->x) * (e2->vert->x - e3->vert->x) +
				(e1->vert->y - e3->vert->y) * (e2->vert->y - e3->vert->y) +
				(e1->vert->z - e3->vert->z) * (e2->vert->z - e3->vert->z))/(len1 * len2));
	
	double t2 = acos(((e3->vert->x - e1->vert->x) * (e2->vert->x - e1->vert->x) +
				(e3->vert->y - e1->vert->y) * (e2->vert->y - e1->vert->y) +
				(e3->vert->z - e1->vert->z) * (e2->vert->z - e1->vert->z))/(len1 * len3));
				
	double t3 = acos(((e3->vert->x - e2->vert->x) * (e1->vert->x - e2->vert->x) +
				(e3->vert->y - e2->vert->y) * (e1->vert->y - e2->vert->y) +
				(e3->vert->z - e2->vert->z) * (e1->vert->z - e2->vert->z))/(len2 * len3));
				
	return MIN(t1, MIN(t2, t3));
}

/**
* Perform edge flipping to obtain a locally delaunay triangulation around.
*/
void localDelaunay(Vertex *v) {
	Edge *e;
	int found = 0;
	while(1) {
		found = 0;
		e = v->edge;
		do {
			double angle1 = MIN(minAngle(e), minAngle(e->pair));
			edgeFlip(e);
			double angle2 = MIN(minAngle(e), minAngle(e->pair));
			if(angle1 >= angle2 || abs(angle1 - angle2) < 1e-4) {
				edgeFlip(e);
			}
			else {
				found = 1;
				break;
			}
			e = e->pair->prev;
		} while(e != v->edge);
		if(!found) break;
	}
}

/**
 * Recalculate edge removal costs for all edges adjacent to v
 */
void recalculate(Mesh *m, Vertex *v) {
	Edge *edge = v->edge;
	Edge *second;
	do {
		second = edge->pair;
		do {
			recalculateKey(m->heap, second);
			recalculateKey(m->heap, second->pair);
			second = second->pair->prev;
		} while(second != edge->pair);
		edge = edge->pair->prev;
	} while(edge != v->edge);
}

/**
* Simple edge removal cost as in lecture notes. Dihedral angle between triangles combined with length of the edge joining them
*/
float simpleCost(Edge *e) {
	float normal1[3], normal2[3];
	float dx, dy, dz;
	faceNormal(e->face, normal1);
	faceNormal(e->pair->face, normal2);
	dx = e->vert->x - e->pair->vert->x;
	dy = e->vert->y - e->pair->vert->y;
	dz = e->vert->z - e->pair->vert->z;
	return sqrt(dx * dx + dy * dy + dz * dz);
	return acos(normal1[0] * normal2[0] + normal1[1] * normal2[1] + normal1[2] * normal2[2]) + sqrt(dx * dx + dy * dy + dz * dz);
}

/**
* Melax edge cost per linked paper. 
*/
float melaxCost(Edge *e) {
	float normal1[3], normal2[3];
	float dx, dy, dz;
	float minCurv;
	float curvature = 0.0f;
	Edge *edge2;
	
	Edge *edge = e;
	do {
		minCurv = 1.0f;
		edge2 = e;
		do {
			faceNormal(edge->face, normal1);
			faceNormal(edge2->face, normal2);
			float dotprod = normal1[0] * normal2[0] + normal1[1] * normal2[1] + normal1[2] * normal2[2];
			minCurv = MIN(minCurv, (1.0f - dotprod)/2.0f);
			edge2 = edge2->pair;
		} while(edge2 != e);
		curvature = MAX(curvature, minCurv);
		edge = edge->pair->prev;
	} while(edge != e);

	dx = e->vert->x - e->pair->vert->x;
	dy = e->vert->y - e->pair->vert->y;
	dz = e->vert->z - e->pair->vert->z;
	
	return curvature * sqrt(dx * dx + dy * dy + dz * dz);
}


/**
* Garland edge removal cost..
*/
float garlandCost(Edge *e) {
	__UNUSED(e);
	return 0.0f;
}


/**
* Determine if the edge e is collapsable without causing topology errors
*/
int collapsable(Edge *e) {
	Edge *ring1, *ring2;
	Vertex *a, *b;
	/* Case (a), edge belongs to a triangle, where the other two edges are boundary edges 
	   This shouldn't happen for manifolds */
	//if(e->next->pair == NULL && e->prev->pair == NULL) return 0;
	//if(e->pair != NULL && e->pair->next->pair == NULL && e->pair->prev->pair == NULL) return 0;
	
	/* Case (b), incident vertices are boundary vertices but edge is not a boundary edge
		This shouldn't happen for manifolds */
	//if(e->pair != NULL && boundaryVertex(e->vert) && boundaryVertex(e->pair->vert)) return 0;
	
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

int reduce(Mesh *m) {
	Edge *e;
	Vertex *v;
	
	e = removeMin(m->heap);
	if(e == NULL) return 0;
	
	v = collapseEdge(m, e);
	localDelaunay(v);
	recalculate(m, v);
	//recalculate(m, v);
	
#ifdef DEBUG
	if(!verifyHeap(m->heap)) {
		printf("Heap consistency error!\n");
	}
#endif
	return 1;
}

Vertex *collapseEdge(Mesh *m, Edge *e) {
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
	
	p->x = (p->x + e->vert->x)/2.0f;
	p->y = (p->y + e->vert->y)/2.0f;
	p->z = (p->z + e->vert->z)/2.0f;
	
	deleteEdge(m, b1);
	deleteEdge(m, d1);
	deleteEdge(m, b2);
	deleteEdge(m, d2);
	deleteFace(m, e->face);
	deleteFace(m, e->pair->face);
	deleteVert(m, e->vert);
	deleteEdge(m, e->pair);
	deleteEdge(m, e);
	
	return p;
}
