#include "meshio.h"

#define MAP_CAPACITY 100000000
#define PROGRESS_RATE 0.1

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? (-(a)) : (a))

HashMap* initMap(int capacity) {
	int i;
	HashMap* map = malloc(sizeof(HashMap));
	map->modulus = capacity;
	map->map = malloc(capacity * sizeof(MapNode*));
	for(i = 0; i < capacity; i++) map->map[i] = NULL;
	return map;
}

void destroyChain(MapNode* node) {
	while(node != NULL) {
		MapNode *next = node->next;
		free(node);
		node = next;
	}
}

void destroyMap(HashMap* map) {
	int i;
	for(i = 0; i < map->modulus; i++) destroyChain(map->map[i]);
	free(map->map);
	free(map);
}

int edgeEqual(EdgeID id1, EdgeID id2) {
	int min1 = MIN(id1.v1, id1.v2);
	int min2 = MIN(id2.v1, id2.v2);
	int max1 = MAX(id1.v1, id1.v2);
	int max2 = MAX(id2.v1, id2.v2);
	return min1 == min2 && max1 == max2;
}

int edgeHash(HashMap *map, EdgeID id) {
	int min = MIN(id.v1, id.v2);
	int max = MAX(id.v1, id.v2);
	return (min * (unsigned long long int)997 + max) % map->modulus;
}

/**
* Associate the key with a value in the given map.
* This will not change an existing key and should only
* be used for insertion (which is assumed to be the case
* for constructing a manifold mesh).
*/
void mapPut(HashMap *map, EdgeID key, Edge *value) {
	int hash = edgeHash(map, key);
	MapNode *newNode = malloc(sizeof(MapNode));
	newNode->next = map->map[hash];
	newNode->key.v1 = key.v1;
	newNode->key.v2 = key.v2;
	newNode->value = value;
	map->map[hash] = newNode;
}

Edge* mapGet(HashMap* map, EdgeID key) {
	int hash = edgeHash(map, key);
	MapNode *node = map->map[hash];
	while(node != NULL) {
		if(edgeEqual(node->key, key)) return node->value;
		node = node->next;
	}
	return NULL;
}

/**
* Given a filename, read an OFF mesh from that file
* into a Mesh object. The Mesh object is a winged edge
* data structure and should be completely filled.
*/
Mesh* readMesh(char* fileName) {
	FILE *f;
	char header[3];
	int numVertices, numFaces, numEdges;
	Vertex **verts;
	Face **faces;
	Edge **edges;
	HashMap* edgeMap;
	int *visited;
	int vCount, v1, v2, v3;
	EdgeID ei1, ei2, ei3;
	Edge *edge1, *edge2, *edge3;
	Edge *e1p, *e2p, *e3p;
	Mesh *m;
	int i, foundPairs;
	double progress = -1.0f, curProgress = 0.0f;
	
	f = fopen(fileName, "r");
	if(f == NULL) {
		printf("Could not open file %s for reading, does it exist?\n", fileName);
		exit(1);
	}
	fscanf(f, "%s", header);
	if(strncmp(header, "OFF", 3)) {
		printf("Model file %s is not in object file format (OFF).\n", fileName);
		exit(2);
	}
	
	fscanf(f, "%d %d %d", &numVertices, &numFaces, &numEdges);
	
	verts = malloc(numVertices * sizeof(Vertex*));
	faces = malloc(numFaces * sizeof(Face*));
	edges = malloc(3 * numFaces * sizeof(Edge*));
	
	printf("Loading %d verticies...\n", numVertices);
	for(i = 0; i < numVertices; i++) {
		curProgress = i/(float)numVertices;
		if(curProgress - progress >= PROGRESS_RATE) {
			progress = curProgress;
			printf("%d%% complete.\n", (int)(progress * 100));
		}
		verts[i] = malloc(sizeof(Vertex));
		verts[i]->index = i;
		verts[i]->iindex = i;
		fscanf(f, "%f %f %f", &(verts[i]->x), &(verts[i]->y), &(verts[i]->z));
	}
	printf("100%% complete.\n");
	
	progress = -1.0f;
	visited = malloc(numVertices * sizeof(int));
	for(i = 0; i < numVertices; i++) visited[i] = 0;
	edgeMap = initMap(MAP_CAPACITY);
	foundPairs = 0;
	printf("Loading %d faces...\n", numFaces);
	for(i = 0; i < numFaces; i++) {
		curProgress = i/(float)numFaces;
		if(curProgress - progress >= PROGRESS_RATE) {
			progress = curProgress;
			printf("%d%% complete.\n", (int)(progress * 100));
		}
		fscanf(f, "%d", &vCount);
		if(vCount != 3) {
			printf("Non-triangle meshes are not supported.");
			exit(3);
		}
		fscanf(f, "%d %d %d", &v1, &v2, &v3);
		if(v1 < 0 || v2 < 0 || v3 < 0) {
			printf("Invalid vertex specified in mesh file %s. Indexing starts at 0.\n", fileName);
			exit(4);
		}
		else if(v1 >= numVertices || v2 > numVertices || v3 > numVertices) {
			printf("Invalid vertex specified in mesh file %s. Indexing ends at %d.\n", fileName, numVertices - 1);
			exit(5);
		}
		
		ei1.v1 = v1;
		ei1.v2 = v2;
		ei2.v1 = v2;
		ei2.v2 = v3;
		ei3.v1 = v3;
		ei3.v2 = v1;
		
		faces[i] = malloc(sizeof(Face));
		faces[i]->index = i;
		faces[i]->iindex = i;
		
		edge1 = malloc(sizeof(Edge));
		edge2 = malloc(sizeof(Edge));
		edge3 = malloc(sizeof(Edge));
		edges[3 * i] = edge1;
		edges[3 * i + 1] = edge2;
		edges[3 * i + 2] = edge3;
		
		edge1->index = 3 * i;
		edge1->vert = verts[v2];
		edge1->face = faces[i];
		edge1->next = edge2;
		edge1->prev = edge3;
		edge1->pair = NULL;
		
		edge2->index = 3 * i + 1;
		edge2->vert = verts[v3];
		edge2->face = faces[i];
		edge2->next = edge3;
		edge2->prev = edge1;
		edge2->pair = NULL;
		
		edge3->index = 3 * i + 2;
		edge3->vert = verts[v1];
		edge3->face = faces[i];
		edge3->next = edge1;
		edge3->prev = edge2;
		edge3->pair = NULL;
		
		faces[i]->edge = edge1;
		
		if(!visited[v1]) {
			visited[v1] = 1;
			verts[v1]->edge = edge3;
		}
		if(!visited[v2]) {
			visited[v2] = 1;
			verts[v2]->edge = edge1;
		}
		if(!visited[v3]) {
			visited[v3] = 1;
			verts[v3]->edge = edge2;
		}
		
		e1p = mapGet(edgeMap, ei1);
		if(e1p == NULL) mapPut(edgeMap, ei1, edge1);
		else {
			edge1->pair = e1p;
			e1p->pair = edge1;
			foundPairs += 2;
		}
		
		e2p = mapGet(edgeMap, ei2);
		if(e2p == NULL) mapPut(edgeMap, ei2, edge2);
		else {
			edge2->pair = e2p;
			e2p->pair = edge2;
			foundPairs += 2;
		}
		
		e3p = mapGet(edgeMap, ei3);
		if(e3p == NULL) mapPut(edgeMap, ei3, edge3);
		else {
			edge3->pair = e3p;
			e3p->pair = edge3;
			foundPairs += 2;
		}
	}
	if(foundPairs != numFaces * 3) {
		printf("Mesh in file %s is non-manifold. %d\n", fileName, foundPairs);
		exit(6);
	}
	printf("100%% complete.\n");
	
	destroyMap(edgeMap);
	fclose(f);
	free(visited);
	
	m = initMesh(numVertices, numFaces, 3 * numFaces, verts, faces, edges);
	return m;
}

void printMesh(Mesh *m, FILE *f) {
	fprintf(f, "OFF\n");
	fprintf(f, "%d %d 0\n", m->numVertices, m->numFaces);
	for(int i = 0; i < m->numVertices; i++) {
		fprintf(f, "%f %f %f\n", m->verts[i]->x,  m->verts[i]->y,  m->verts[i]->z);
	}
	for(int i = 0; i < m->numFaces; i++) {
		fprintf(f, "3 %d %d %d\n", m->faces[i]->edge->vert->index, m->faces[i]->edge->next->vert->index, m->faces[i]->edge->next->next->vert->index);
	}
	
	/** Test Stuff **/
	/*
	fprintf(f, "\n");
	for(int i = 0; i < m->numVertices; i++) {
		fprintf(f, "%d: ", m->verts[i]->index);
		fprintf(f, " (%d -> %d)\n", m->verts[i]->index, m->verts[i]->edge->pair->vert->index);
	}*/
}
