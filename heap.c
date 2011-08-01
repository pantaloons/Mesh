#include "heap.h"

Heap *initHeap(Mesh *m, float (*f)(Edge*), int (*test)(Edge*)) {
	Heap *h = malloc(sizeof(Heap));
	EdgeNode *node;
	int i;
	
	h->capacity = m->numEdges;
	h->size = 0;
	h->func = f;
	h->test = test;
	h->heap = malloc(h->capacity * sizeof(EdgeNode*));
	
	/* TODO: We can do this in O(n), as is O(n logn) which isn't half bad, (factor of 2-8x) */
	for(i = 0; i < m->numEdges; i++) {
		if((*h->test)(m->edges[i])) { /* Edge is collapsable */
			node = heapInsert(h, m->edges[i]);
			m->edges[i]->heapNode = node;
		}
		else m->edges[i]->heapNode = NULL;
	}
	
	return h;
}

void destroyHeap(Heap *h) {
	int i;
	for(i = 0; i < h->size; i++) free(h->heap[i]);
	free(h->heap);
	free(h);
}

void recalculateKey(Heap *h, Edge *edge) {
	float cost = (*h->func)(edge);
	if(edge->heapNode != NULL && !(*h->test)(edge)) removeEdge(h, edge);
	else if(edge->heapNode == NULL) heapInsert(h, edge);
	else if(cost == edge->heapNode->cost) return;
	else {
		if(cost < edge->heapNode->cost) {
			edge->heapNode->cost = cost;
			siftup(h, edge->heapNode->index);
		}
		else {
			edge->heapNode->cost = cost;
			siftdown(h, edge->heapNode->index);
		}
	}
}

EdgeNode *heapInsert(Heap *h, Edge *edge) {
	float cost = (*h->func)(edge);
	EdgeNode *current = malloc(sizeof(EdgeNode));
	current->cost = cost;
	current->edge = edge;
	current->index = h->size;
	
	h->heap[h->size] = current;
	h->size++;
	siftup(h, h->size - 1);
	
	return current;
}

Edge *removeMin(Heap *h) {
	Edge *edge;
	if(h->size == 0) return NULL;
	edge = h->heap[0]->edge;
	
	h->heap[0] = h->heap[h->size - 1];
	h->heap[0]->index = 0;
	h->size--;
	siftdown(h, 0);
	
	return edge;
}

void removeEdge(Heap *h, Edge *e) {
	if(h->size == 0 || e->heapNode == NULL) return;
	
	h->heap[e->heapNode->index] = h->heap[h->size - 1];
	h->heap[e->heapNode->index]->index = e->heapNode->index;
	h->size--;
	siftdown(h, e->heapNode->index);
}

void siftdown(Heap *h, int index) {
	EdgeNode *current = h->heap[index];
	int swapLeft, swapRight;
	while(1) {
		swapLeft = index * 2 + 1 < h->size && current->cost > h->heap[index * 2 + 1]->cost;
		if(swapLeft) {
			h->heap[index] = h->heap[index * 2 + 1];
			h->heap[index]->index = index;
			h->heap[index * 2 + 1] = current;
			index = index * 2 + 1;
			h->heap[index]->index = index;
			continue;
		}
		swapRight = index * 2 + 2 < h->size && current->cost > h->heap[index * 2 + 2]->cost;
		if(swapRight) {
			h->heap[index] = h->heap[index * 2 + 2];
			h->heap[index]->index = index;
			h->heap[index * 2 + 2] = current;
			index = index * 2 + 2;
			h->heap[index]->index = index;
			continue;
		}
		break;
	}
}

void siftup(Heap *h, int index) {
	EdgeNode *current = h->heap[index], *temp;
	while(current->index > 0 && h->heap[(current->index - 1)/2]->cost > current->cost) {
		temp = h->heap[(current->index - 1)/2];
		temp->index = current->index;
		h->heap[(current->index - 1)/2] = current;
		h->heap[current->index] = temp;
		current->index = (current->index - 1)/2;
	}
}
