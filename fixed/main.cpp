#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RayTrack.h"

const int HEIGHT = 800;
const int WIDTH = 800;

static glm::dvec3 screenPixels[HEIGHT][WIDTH];

int i = 0;

void setsScreenPixels()
{
	std::vector<Sphere> spheres;
	Sphere sphere0;
	sphere0.center = glm::dvec3(0, 0, 0);
	sphere0.diffuse = glm::dvec3(0.2, 0.3, 0.6);
	sphere0.specular = glm::dvec3(0.8, 0.8, 0.8);
	sphere0.isLight = true;
	sphere0.emission = glm::dvec3(0.0, 0.0, 0.0);
	sphere0.r = 0.3;
	sphere0.r2 = 0.09;

	spheres.push_back(sphere0);

	sphere0.center = glm::dvec3(0, 0.7, 0);
	sphere0.emission = glm::dvec3(0.8, 0.8, 0.8);

	spheres.push_back(sphere0);

	sphere0.center = glm::dvec3(0, -0.7, 0);
	sphere0.emission = glm::dvec3(0.3, 0.1, 0.2);
	spheres.push_back(sphere0);



	glm::dvec3 ambient = glm::dvec3(0.1, 0.1, 0.1);

	glm::dvec3 background = glm::dvec3(0.05, 0.05, 0.05);

	RayTracker tracker(spheres, ambient, background);

	glm::dvec3 cameraFront(0, 0, -1);
	glm::dvec3 cameraRight(1, 0, 0);
	glm::dvec3 cameraUp(0, 1, 0);
	double focus = 1;
	glm::dvec3 cameraPos(0, 0, 1.5 + i*0.1);
	i++;



	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			Ray ray{
				glm::dvec3((double)(2 * i - HEIGHT) / HEIGHT, (double)(2 * j - WIDTH) / WIDTH, 0) + focus*cameraFront,
				cameraPos + glm::dvec3((double)(2 * i - HEIGHT) / HEIGHT, (double)(2 * j - WIDTH) / WIDTH, 0) + focus*cameraFront
			};
			screenPixels[i][j] = tracker.track(ray, 1);
		}
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	setsScreenPixels();

	glBegin(GL_POINTS);
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			glColor3dv(glm::value_ptr(screenPixels[i][j]));
			glVertex2i(i, j);
		}
	}
	glEnd();
	glutSwapBuffers();
}

void init(void)
{
	glClearColor(0.05, 0.05, 0.05, 0.0);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, HEIGHT, 0.0, WIDTH);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(HEIGHT, WIDTH);
	glutCreateWindow("point");
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;
}