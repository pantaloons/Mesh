#ifdef _WIN32
#include <windows.h>
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
#define MODEL_FILE "objects/camel.off"

int width = WINDOW_START_WIDTH;
int height = WINDOW_START_HEIGHT;

Mesh* mesh;
GLfloat lightMat[] = {1.0, 0.0, 0.0, 1.0}; 
GLfloat lightPos[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

void glInit() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightMat);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void reshape(GLint newWidth, GLint newHeight) {
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, width / (float)height, 1.0f, 1000.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-100, 100, -100, 0, 0, 0, 0, 1, 0);
	
	glutPostRedisplay();
}

void render(void) {
	int i;
	glPushMatrix();
	glScalef(90, 90, 90);
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

void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	render();
	
	glutSwapBuffers();
}

void deallocate(void) {
	destroyMesh(mesh);
}

int main(int argc, char** argv) {
	mesh = readMesh(MODEL_FILE);
	
	atexit(deallocate);
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Mesh simplification");
	
	glInit();
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	
	glutMainLoop();
	
	return 0;
}
