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

#include "meshio.h"

#define WINDOW_START_WIDTH 640
#define WINDOW_START_HEIGHT 480
#define SCENE_SPEED 1.0f
#define CLOCK_RATE 1000
#define MODEL_FILE "objects/cube.off"

int width = WINDOW_START_WIDTH;
int height = WINDOW_START_HEIGHT;

unsigned long lastTick;

Mesh* mesh;

GLfloat lightMat[] = {1.0, 0.0, 0.0, 1.0}; 
GLfloat lightPos[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

int yrot = 0;

unsigned long getTime() {
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + tv.usec/1000.0) + 0.5;
#endif
}

void glInit() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	
	glShadeModel(GL_SMOOTH);

	glEnable(GL_NORMALIZE);
	glClearColor(0.5, 0.5, 0.5, 1.0);
	
	lastTick = getTime();
}

void reshape(GLint newWidth, GLint newHeight) {
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
	
	glutPostRedisplay();
}

void render(void) {
	int i;
	glPushMatrix();
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);  
	//glScalef(80, 80, 80);
	glScalef(10, 10, 10);
	glBegin(GL_TRIANGLES);
	for(i = 0; i < mesh->numFaces; i++) {
		Edge* edge = mesh->faces[i]->edge;
		do {
			Vertex* vert = edge->vert;
			glVertex3f(vert->x, vert->y, vert->z);
			edge = edge->next;
		}
		while(edge != mesh->faces[i]->edge);
	}
	glPopMatrix();
	glEnd();
}

void draw(float timeDelta) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, width / (float)height, 1.0f, 1000.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightMat);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	gluLookAt(-50, 40, -30, -25, 10, 0, 0, 1, 0);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
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
	switch(key) {
		case '1':
			printf("Deleting edge: (%d, %d)\n", mesh->edges[0]->vert->index, mesh->edges[0]->pair->vert->index);
			collapseEdge(mesh, mesh->edges[0]);
			break;
		default: break;
	}
	glutPostRedisplay();
}

void specialInput(int k, int x, int y) {
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
	
	atexit(deallocate);
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Mesh Simplification");
	
	glInit();
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(tick);
	glutIdleFunc(tick);
	glutSpecialFunc(specialInput);
	glutKeyboardFunc(keyboardInput);
	
	glutMainLoop();
	
	return 0;
}
