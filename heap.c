#include "heap.h"

Heap *initHeap(Mesh *m, float (*f)(Edge*), int (*test)(Edge*)) {
	Heap *h = (Heap*)malloc(sizeof(Heap));
	EdgeNode *node;
	int i;
	
	h->capacity = m->numEdges;
	h->size = 0;
	h->func = f;
	h->test = test;
	h->heap = (EdgeNode**)malloc(h->capacity * sizeof(EdgeNode*));
	
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
	EdgeNode *node;
	if(edge->heapNode != NULL && !(*h->test)(edge)) removeEdge(h, edge);
	else if(edge->heapNode == NULL) {
		if((*h->test)(edge)) {
			node = heapInsert(h, edge);
			edge->heapNode = node;
		}
	}
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
	EdgeNode *current;
	if(!(*h->test)(edge)) return NULL;
	current = (EdgeNode*)malloc(sizeof(EdgeNode));
	current->cost = cost;
	current->edge = edge;
	current->index = h->size;
	
	h->heap[h->size] = current;
	h->size += 1;
	siftup(h, h->size - 1);
	
	return current;
}

Edge *removeMin(Heap *h) {
	Edge *edge;
	if(h->size == 0) return NULL;
	edge = h->heap[0]->edge;
	edge->heapNode = NULL;
	
	h->heap[0] = h->heap[h->size - 1];
	h->heap[0]->index = 0;
	h->size--;
	siftdown(h, 0);
	
	return edge;
}

void removeEdge(Heap *h, Edge *e) {
	if(h->size == 0 || e->heapNode == NULL) return;
	float oldCost = e->heapNode->cost;
	h->heap[e->heapNode->index] = h->heap[h->size - 1];
	h->heap[e->heapNode->index]->index = e->heapNode->index;
	h->size -= 1;
	float newCost = h->heap[e->heapNode->index]->cost;
	if(newCost > oldCost) {
		siftdown(h, e->heapNode->index);
	}
	else if(newCost < oldCost) {
		siftup(h, e->heapNode->index);
	}
	e->heapNode = NULL;
}

void siftdown(Heap *h, int index) {
	EdgeNode *temp;
	while(1) {
		int left = index * 2 + 1;
		int right = index * 2 + 2;
		int largest = index;
		if(left < h->size && h->heap[left]->cost < h->heap[index]->cost) largest = left;
		if(right < h->size && h->heap[right]->cost < h->heap[largest]->cost) largest = right;
		if(largest != index) {
			temp = h->heap[index];
			h->heap[index] = h->heap[largest];
			h->heap[largest] = temp;
			h->heap[index]->index = index;
			h->heap[largest]->index = largest;
			index = largest;
			continue;
		}
		break;
	}
}

void siftup(Heap *h, int index) {
	EdgeNode *temp;
	while(index > 0 && h->heap[(index - 1)/2]->cost > h->heap[index]->cost) {
		temp = h->heap[index];
		h->heap[index] = h->heap[(index - 1)/2];
		h->heap[(index - 1)/2] = temp;
		h->heap[index]->index = index;
		h->heap[(index - 1)/2]->index = (index - 1)/2;
		index = (index - 1)/2;
	}
}

int check(Heap *h, int index) {
	if(index >= h->size) return 1;
	if(h->heap[(index - 1)/2]->cost > h->heap[index]->cost) return 0;
	if(h->heap[index]->edge->heapNode != h->heap[index]) return 0;
	return check(h, 2 * index + 1) && check(h, 2 * index + 2);
}

int verifyHeap(Heap *h) {
	if(h->heap[0]->edge->heapNode != h->heap[0]) return 0;
	if(h->size == 0) return 1;
	return check(h, 1) && check(h, 2);
}
