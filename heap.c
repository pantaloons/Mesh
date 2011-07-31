#include "heap.h"

Heap *heapInit(Mesh *m, float (*f)(Edge*)) {
	Heap *h = malloc(sizeof(Heap));
	int i;
	
	h->capacity = m->numEdges;
	h->size = m->numEdges;
	h->func = f;
	h->heap = malloc(h->capacity * sizeof(Edge*));
	
	/* TODO: We can do this in O(n), as is O(n logn) which isn't half bad, (factor of 2-8x) */
	for(i = 0; i < capacity; i++) heapInsert(h, m->edges[i]);
	return h;
}

void heapInsert(Heap *h, Edge *edge) {
	float cost = (*h->func)(edge);
	EdgeNode *current = malloc(sizeof(EdgeNode));
	int index = h->size;
	current->cost = cost;
	current->edge = edge;
	h->heap[h->size] = current;

	while(index > 0 && h->heap[(index - 1)/2]->cost > cost) {
		EdgeNode *temp = h->heap[(index - 1)/2];
		h->heap[(index - 1)/2] = current;
		h->heap[index] = temp;
		index = (index - 1)/2;
	}
	
	h->size++;
}

Edge *removeMin(Heap *h) {
	int index = 0;
	int swapLeft, swapRight;
	Edge *result;
	EdgeNode *current;
	
	if(h->size == 0) return NULL;
	result = h->heap[0]->edge;
	current = h->heap[h->size - 1];
	
	while(1) {
		swapLeft = index * 2 + 1 < h->size && current->cost > h->heap[index * 2 + 1]->cost;
		if(swapLeft) {
			h->heap[index] = h->heap[index * 2 + 1];
			h->heap[index * 2 + 1] = current;
			index = index * 2 + 1;
			continue;
		}
		swapRight = index * 2 + 2 < h->size && current->cost > h->heap[index * 2 + 2]->cost;
		if(swapRight) {
			h->heap[index] = h->heap[index * 2 + 2];
			h->heap[index * 2 + 2] = current;
			index = index * 2 + 2;
			continue;
		}
		break;
	}
	
	h->size--;
	return result;
}