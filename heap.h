#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdlib.h>

typedef struct _edge {
	int index;
	struct _vertex *vert;
	struct _face *face;
	struct _edge *prev, *next, *pair;
} Edge;

typedef struct _edgenode {
	float cost;
	Edge* edge;
} EdgeNode;

typedef struct _edgeheap {
	int capacity;
	int size;
	EdgeNode **heap;
	float (*func)(Edge*); /* Pointer to edge evaluation function */
} Heap;

Heap *heapInit(Mesh *m, float (*f)(Edge*) );
void heapInsert(Heap *h, Edge *edge);
Edge *removeMin(Heap *h);

#endif