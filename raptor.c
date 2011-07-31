#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef _WIN32
#include "lib/glut.h"
#else
#include <GL/glut.h>
#endif

#include "heap.h"
#include "meshio.h"

#define __UNUSED(x) (void)x;

#define WINDOW_START_WIDTH 640
#define WINDOW_START_HEIGHT 480
#define SCENE_SPEED 1.0f
#define CLOCK_RATE 1000
#define MODEL_FILE "objects/eight.off"

int width = WINDOW_START_WIDTH;
int height = WINDOW_START_HEIGHT;

unsigned long lastTick;
int lines;

Mesh *mesh;
Heap *heap;

GLfloat lightMat[] = {1.0, 0.0, 0.0, 1.0}; 
GLfloat lightPos[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

int yrot = 0;

unsigned long getTime() {
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec/1000.0) + 0.5;
#endif
}

void glInit() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightMat);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_NORMALIZE);
	glClearColor(0.5, 0.5, 0.5, 1.0);
	
	lastTick = getTime();
	lines = 0;
}

void reshape(GLint newWidth, GLint newHeight) {
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
	
	glutPostRedisplay();
}

void render(void) {
	int i;
	Edge *edge;
	float normal[3];
	glPushMatrix();
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);  
	glScalef(80, 80, 80);
	//glScalef(10, 10, 10);
	glBegin(GL_TRIANGLES);
	for(i = 0; i < mesh->numFaces; i++) {
		faceNormal(mesh->faces[i], normal);
		edge = mesh->faces[i]->edge;
		do {
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(edge->vert->x, edge->vert->y, edge->vert->z);
			edge = edge->next;
		}
		while(edge != mesh->faces[i]->edge);
	}
	glPopMatrix();
	glEnd();
}

void draw(float timeDelta) {
	__UNUSED(timeDelta);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, width / (float)height, 1.0f, 1000.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(-50, 40, -30, -25, 10, 0, 0, 1, 0);
	
	if(lines) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	render();
	
	glutSwapBuffers();
}

void tick(void) {
	unsigned long curTick = getTime();
	unsigned long delta = curTick - lastTick;
	
	draw(delta);
	
	lastTick = curTick;
}

void deallocate(void) {
	destroyMesh(mesh);
}

void keyboardInput(unsigned char key, int x, int y) {
	int index;
	__UNUSED(x);
	__UNUSED(y);
	switch(key) {
		case '1': {
			index = 0;
			while(!collapsable(mesh->edges[index]) && index < mesh->numEdges)
				index++;
			if(index == mesh->numEdges) {
				printf("No collapsable edges left.\n");
				break;
			}
			collapseEdge(mesh, mesh->edges[index]);
			break;
		}
		case 'l':
			lines = 1 - lines;
			break;
		case 'p':
			printMesh(mesh, stdout);
			break;
		default: break;

	}
	glutPostRedisplay();
}

void specialInput(int k, int x, int y) {
	__UNUSED(x);
	__UNUSED(y);
	switch(k) {
		case GLUT_KEY_LEFT:
			yrot = (yrot + 5) % 360;
			break;
		case GLUT_KEY_RIGHT:
			yrot = (yrot - 5) % 360;
			break;
		default: break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	mesh = readMesh(MODEL_FILE);
	heap = heapInit(mesh, simpleCost);
	
	atexit(deallocate);
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Mesh Simplification");
	
	glInit();
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(tick);
	//glutIdleFunc(tick);
	glutSpecialFunc(specialInput);
	glutKeyboardFunc(keyboardInput);
	
	glutMainLoop();
	
	return 0;
}
