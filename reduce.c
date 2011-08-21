#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "heap.h"
#include "meshio.h"

#define __UNUSED(x) (void)x;
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define WINDOW_START_WIDTH 640
#define WINDOW_START_HEIGHT 480
#define SCENE_SPEED 1.0f
#define CLOCK_RATE 1000
#define MODEL_FILE "camel.off"

char* fileName;
int width = WINDOW_START_WIDTH;
int height = WINDOW_START_HEIGHT;

unsigned long lastTick;
int lines;

float dimensions[6];
Mesh *mesh;
Heap *heap;

GLfloat lightMat[] = {1.0, 0.0, 0.0, 1.0}; 
GLfloat lightPos[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

int xrot = 0;
int yrot = 0;
int zoom = 0;

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
	glTranslatef((dimensions[0] + dimensions[1])/2.0f, (dimensions[2] + dimensions[3])/2.0f, (dimensions[4] + dimensions[5])/2.0f);
	glRotatef(yrot, 1.0f, 0.0f, 0.0f);
	glRotatef(xrot, 0.0f, 1.0f, 0.0f);
	glTranslatef(-(dimensions[0] + dimensions[1])/2.0f, -(dimensions[2] + dimensions[3])/2.0f, -(dimensions[4] + dimensions[5])/2.0f);
	
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
	gluPerspective(70.0f, width / (float)height, 0.01f, 10000.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float dist = MAX(MAX(dimensions[1] - dimensions[0], dimensions[3] - dimensions[2]), dimensions[5] - dimensions[4]);
	glTranslatef(0, 0, zoom * dist/5.0f);
	gluLookAt((dimensions[0] + dimensions[1])/2.0f, (dimensions[2] + dimensions[3])/2.0f, dimensions[5] + 1.0f * dist,
			   (dimensions[0] + dimensions[1])/2.0f, (dimensions[2] + dimensions[3])/2.0f, (dimensions[4] + dimensions[5])/2.0f,
			   0, 1, 0);
	
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

void reset() {
	mesh = readMesh(fileName, dimensions);
}

void keyboardInput(unsigned char key, int x, int y) {
	__UNUSED(x);
	__UNUSED(y);
	switch(key) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			int initEdges = mesh->numEdges;
			int initPolys = mesh->numFaces;
			int targetEdges = MAX(6, (1.0f - 0.1f * (int)(key - '0')) * mesh->numEdges);
			while(mesh->numEdges > targetEdges) {
				if(!reduce(mesh)) break;
			}
			printf("Mesh successfully reduced by %c0%%. From %d to %d edges, %d to %d polys.\n",
				key, initEdges, mesh->numEdges, initPolys, mesh->numFaces);
			break;
		}
		case '`':
			reduce(mesh);
			printf("Mesh successfully reduced by one vertex.\n");
			break;
		case 'v':
			lines = 1 - lines;
			printf("Display mode changed. ");
			if(lines) printf("Drawing with GL_LINES.\n");
			else printf("Drawing with GL_TRIANGLES.\n");
			break;
		case 'p':
			printMesh(mesh, stdout);
			break;
		case 'q':
			exit(0);
			break;
		case 'r':
			reset();
			printf("Mesh successfully reset.\n");
			break;
		case 'm':
			changeCostFunc(mesh, melaxCost);
			printf("Cost function changed to Melax.\n");
			break;
		case 's':
			changeCostFunc(mesh, simpleCost);
			printf("Cost function changed to Simple.\n");
			break;
		case '+':
			zoom += 1;
			break;
		case '-':
			zoom -= 1;
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
			xrot = (xrot + 5) % 360;
			break;
		case GLUT_KEY_RIGHT:
			xrot = (xrot - 5) % 360;
			break;
		case GLUT_KEY_UP:
			yrot = (yrot - 5);
			break;
		case GLUT_KEY_DOWN:
			yrot = (yrot + 5);
			break;
		default: break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	if(argc <= 1) {
		fileName = malloc((strlen(MODEL_FILE) + 1) * sizeof(char));
		fileName[0] = 0;
		strcat(fileName, MODEL_FILE);
	}
	else {
		fileName = malloc((strlen(argv[1]) + 1) * sizeof(char));
		fileName[0] = 0;
		strcat(fileName, argv[1]);
	}
	
	mesh = readMesh(fileName, dimensions);
	//keyboardInput('9', 0, 0);
	
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
