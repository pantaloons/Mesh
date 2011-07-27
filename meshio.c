#include "meshio.h"

#define MAP_CAPACITY 100000000

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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

void mapPut(HashMap *map, EdgeID key, Edge *value) {
	int hash = (key.v1 * 997 + key.v2) % map->modulus;
	MapNode *node = map->map[hash];
	MapNode *newNode;
	while(node != NULL) {
		if(edgeEqual(node->key, key)) {
			node->value = value;
			return;
		}
		node = node->next;
	}
	newNode = malloc(sizeof(MapNode));
	newNode->next = map->map[hash];
	newNode->key.v1 = key.v1;
	newNode->key.v2 = key.v2;
	newNode->value = value;
	map->map[hash] = newNode;
}

Edge* mapGet(HashMap* map, EdgeID key) {
	int hash = (key.v1 * (unsigned long long int)5003 + key.v2) % map->modulus;
	MapNode *node = map->map[hash];
	while(node != NULL) {
		if(edgeEqual(node->key, key)) return node->value;
		node = node->next;
	}
	return NULL;
}

Mesh* readMesh(char* fileName) {
	FILE *f = fopen(fileName, "r");
	char header[3];
	int numVertices, numFaces, numEdges;
	Vertex **verts;
	Face **faces;
	Edge **edges;
	HashMap* edgeMap = initMap(MAP_CAPACITY);
	int *visited;
	int vCount, v1, v2, v3;
	EdgeID ei1, ei2, ei3;
	Edge *edge1, *edge2, *edge3;
	Mesh *m;
	int i;
	
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
	
	for(i = 0; i < numVertices; i++) {
		if(i % 50000 == 0) printf("Loading vert %d\n", i);
		verts[i] = malloc(sizeof(Vertex));
		fscanf(f, "%f %f %f", &(verts[i]->x), &(verts[i]->y), &(verts[i]->z));
	}
	
	visited = malloc(numVertices * sizeof(int));
	for(i = 0; i < numVertices; i++) visited[i] = 0;
	for(i = 0; i < numFaces; i++) {
		if(i % 50000 == 0) printf("Loading face %d\n", i);
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
		
		edge1 = malloc(sizeof(Edge));
		edge2 = malloc(sizeof(Edge));
		edge3 = malloc(sizeof(Edge));
		edges[3 * i] = edge1;
		edges[3 * i + 1] = edge2;
		edges[3 * i + 2] = edge3;
		
		edge1->vert = verts[v2];
		edge1->face = faces[i];
		edge1->next = edge2;
		edge1->prev = edge3;
		
		edge2->vert = verts[v3];
		edge2->face = faces[i];
		edge2->next = edge3;
		edge2->prev = edge1;
		
		edge3->vert = verts[v1];
		edge3->face = faces[i];
		edge3->next = edge1;
		edge3->prev = edge2;
		
		faces[i]->edge = edge1;
		
		if(!visited[v1]) {
			visited[v1] = 1;
			verts[v1]->edge = edge1;
		}
		if(!visited[v2]) {
			visited[v2] = 1;
			verts[v2]->edge = edge1;
		}
		if(!visited[v3]) {
			visited[v3] = 1;
			verts[v3]->edge = edge1;
		}
		
		edge1->pair = mapGet(edgeMap, ei1);
		mapPut(edgeMap, ei1, edge1);
		edge2->pair = mapGet(edgeMap, ei2);
		mapPut(edgeMap, ei2, edge2);
		edge3->pair = mapGet(edgeMap, ei3);
		mapPut(edgeMap, ei3, edge3);
	}
	destroyMap(edgeMap);
	fclose(f);
	free(visited);
	
	m = initMesh(numVertices, numFaces, 3 * numFaces, verts, faces, edges);
	return m;
}
