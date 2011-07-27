#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "meshio.h"

#define WINDOW_START_WIDTH 640
#define WINDOW_START_HEIGHT 480
#define MODEL_FILE "dragon.off"

int width = WINDOW_START_WIDTH;
int height = WINDOW_START_HEIGHT;

void glInit() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

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
	gluPerspective(70.0f, width / (float)height, 1.0f, 100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-25, -25, -25, 0, 0, 0, 0, 1, 0);
}

void render() {
	
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	render();
	
	glutSwapBuffers();
}

int main(void) {
	Mesh* m = readMesh(MODEL_FILE);
	return 0;
}

int main2(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Mesh simplification");
	
	glInit();
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	
	
	glutMainLoop();
	
	return 0;
}
