#ifndef __TYPES_H__
#define __TYPES_H__

struct _edgenode;

typedef struct _edge {
	int index;
	struct _vertex *vert;
	struct _face *face;
	struct _edge *prev, *next, *pair;
	struct _edgenode *heapNode;
} Edge;

typedef struct _edgenode {
	int index;
	float cost;
	Edge* edge;
} EdgeNode;

typedef struct _edgeheap {
	int capacity;
	int size;
	EdgeNode **heap;
	float (*func)(Edge*); /* Pointer to edge evaluation function */
	int (*test)(Edge*); /* Pointer to edge collapsability test function */
} Heap;

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
	Heap *heap;
} Mesh;

#endif
