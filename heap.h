#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdlib.h>
#include "types.h"
#define __UNUSED(x) (void)x;

Heap *initHeap(Mesh *m, float (*f)(Edge*), int (*test)(Edge*));
void destroyHeap(Heap *h);

void recalculateKey(Heap *h, Edge *edge);

EdgeNode *heapInsert(Heap *h, Edge *edge);
Edge *removeMin(Heap *h);
void removeEdge(Heap *h, Edge *e);

void siftdown(Heap *h, int index);
void siftup(Heap *h, int index);

#endif
