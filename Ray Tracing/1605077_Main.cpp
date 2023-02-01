#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <windows.h>
#include <glut.h>

#include "bitmap_image.hpp"

#include "1605077_classes.h"

using namespace std;

#define WINDOW_SIZE 800
// #define pi (2 * acos(0.0))

int drawgrid;
int drawaxes;

double translationInc;
double rotationInc;

Point x, y, z, u, r, l, pos;

int recursionLevel, imageDimension;
int totalObjects, totalLights;

vector<Object *> objects;
vector<Light> lights;

void drawAxes()
{
	if (drawaxes == 1)
	{
		glBegin(GL_LINES);
		{
			glColor3f(1, 0, 0);
			glVertex3f(150, 0, 0);
			glVertex3f(-150, 0, 0);

			glColor3f(0, 1, 0);
			glVertex3f(0, -150, 0);
			glVertex3f(0, 150, 0);

			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 150);
			glVertex3f(0, 0, -150);
		}
		glEnd();
	}
}
void drawGrid()
{
	int i;
	if (drawgrid == 1)
	{
		glColor3f(0.6, 0.6, 0.6); //grey
		glBegin(GL_LINES);
		{
			for (i = -8; i <= 8; i++)
			{

				if (i == 0)
					continue; //SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i * 10, -90, 0);
				glVertex3f(i * 10, 90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i * 10, 0);
				glVertex3f(90, i * 10, 0);
			}
		}
		glEnd();
	}
}

// Camera Movements
void moveForward()
{
	pos = pos.add(l.mult(translationInc));
}
void moveBackward()
{
	pos = pos.add(l.mult(-translationInc));
}
void moveRight()
{
	pos = pos.add(r.mult(translationInc));
}
void moveLeft()
{
	pos = pos.add(r.mult(-translationInc));
}
void moveUp()
{
	pos = pos.add(u.mult(translationInc));
}
void moveDown()
{
	pos = pos.add(u.mult(-translationInc));
}
void rotateLeft()
{
	l = l.rotateArround(u, rotationInc);
	r = r.rotateArround(u, rotationInc);
}
void rotateRight()
{
	l = l.rotateArround(u, -rotationInc);
	r = r.rotateArround(u, -rotationInc);
}
void lookUp()
{
	l = l.rotateArround(r, rotationInc);
	u = u.rotateArround(r, rotationInc);
}
void lookDown()
{
	l = l.rotateArround(r, -rotationInc);
	u = u.rotateArround(r, -rotationInc);
}
void tiltLeft()
{
	u = u.rotateArround(l, rotationInc);
	r = r.rotateArround(l, rotationInc);
}
void tiltRight()
{
	u = u.rotateArround(l, -rotationInc);
	r = r.rotateArround(l, -rotationInc);
}

void capture()
{
	bitmap_image image(imageDimension, imageDimension);

	double planeDistance = (WINDOW_SIZE / 2) / tan(40 * pi / 180);
	Point topLeft = pos.add(l.mult(planeDistance)).add(r.mult(WINDOW_SIZE / -2)).add(u.mult(WINDOW_SIZE / 2));
	double du = WINDOW_SIZE / imageDimension;
	double dv = WINDOW_SIZE / imageDimension;
	topLeft = topLeft.add(r.mult(0.5 * du)).add(u.mult(-0.5 * dv));

	for (int i = 0; i < imageDimension; i++)
	{
		for (int j = 0; j < imageDimension; j++)
		{
			Point currentPixel = topLeft.add(r.mult(j * du)).add(u.mult(-i * dv));

			Point rayDir = currentPixel.add(pos.mult(-1));
			rayDir.normalize();
			Ray ray(pos, rayDir);

			double *color = new double[3];
			double tMin = INFINITY;
			Object *oNear;
			for (int k = 0; k < objects.size(); k++)
			{
				double t = objects[k]->intersect(ray, color, 0);
				if (t < tMin)
				{
					tMin = t;
					oNear = objects[k];
				}
			}
			if (tMin != INFINITY)
			{
				tMin = oNear->intersect(ray, color, 1);
				image.set_pixel(j, i, 255 * color[0], 255 * color[1], 255 * color[2]);
			}

			free(color);
		}
	}

	image.save_image("capture.bmp");
	image.clear();

	cout << "Captured!" << endl;
}

// Control Inputs
void keyboardListener(unsigned char key, int xx, int yy)
{
	switch (key)
	{
	case '0':
		capture();
		break;
	case '1':
		rotateLeft();
		break;
	case '2':
		rotateRight();
		break;
	case '3':
		lookUp();
		break;
	case '4':
		lookDown();
		break;
	case '5':
		tiltRight();
		break;
	case '6':
		tiltLeft();
		break;
	default:
		break;
	}
}
void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		moveForward(); // up arrow key
		//cameraHeight += 3.0;
		break;
	case GLUT_KEY_DOWN: //down arrow key
		//cameraHeight -= 3.0;
		moveBackward();
		break;
	case GLUT_KEY_RIGHT:
		//cameraAngle += 0.03;
		moveRight();
		break;
	case GLUT_KEY_LEFT:
		//cameraAngle -= 0.03;
		moveLeft();
		break;

	case GLUT_KEY_PAGE_UP:
		moveUp();
		break;
	case GLUT_KEY_PAGE_DOWN:
		moveDown();
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;

	default:
		break;
	}
}
void mouseListener(int button, int state, int x, int y)
{ //x, y is the x-y of the screen (2D)
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{ // 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);
	glMatrixMode(GL_MODELVIEW);

	drawAxes();
	// drawGrid();

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->draw();
	}

	for (int i = 0; i < lights.size(); i++)
	{
		lights[i].draw();
	}

	glutSwapBuffers();
}
void animate()
{
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void initializeAxes()
{
	x.x = 1;
	x.y = 0;
	x.z = 0;

	y.x = 0;
	y.y = 1;
	y.z = 0;

	z.x = 0;
	z.y = 0;
	z.z = 1;
}
void initializeCamera()
{
	u.x = 0;
	u.y = 0;
	u.z = 1;

	r.x = -1 / sqrt(2);
	r.y = 1 / sqrt(2);
	r.z = 0;

	l.x = -1 / sqrt(2);
	l.y = -1 / sqrt(2);
	l.z = 0;

	pos.x = 170;
	pos.y = 170;
	pos.z = 40;

	// printf("u: %lf %lf %lf\n", u.x, u.y, u.z);
	// printf("r: %lf %lf %lf\n", r.x, r.y, r.z);
	// printf("l: %lf %lf %lf\n", l.x, l.y, l.z);
}
void init()
{
	//codes for initialization
	drawgrid = 1;
	drawaxes = 1;

	translationInc = 5;
	rotationInc = 0.05;

	//clear the screen
	glClearColor(0, 0, 0, 0);
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	initializeAxes();
	initializeCamera();
}

// File IO
vector<double> split(const string &s, char delim)
{
	vector<double> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim))
	{
		result.push_back(stod(item));
	}

	return result;
}
void loadData()
{
	fstream file;
	file.open("scene.txt", ios::in);

	if (file.is_open())
	{
		string line;
		vector<double> values;

		getline(file, line);
		values = split(line, ' ');
		recursionLevel = values[0];
		cout << "re " << recursionLevel << endl;

		getline(file, line);
		values = split(line, ' ');
		imageDimension = values[0];
		// imageDimension = 100;
		cout << "dim " << imageDimension << endl;

		getline(file, line);

		getline(file, line);
		values = split(line, ' ');
		totalObjects = values[0];
		cout << "objscts " << totalObjects << endl;

		for (int i = 0; i < totalObjects; i++)
		{
			getline(file, line);

			cout << "command " << line << endl;

			if (line == "sphere")
			{
				getline(file, line);
				values = split(line, ' ');
				Point center(values[0], values[1], values[2]);

				getline(file, line);
				values = split(line, ' ');
				double radius = values[0];

				Object *s;
				s = new Sphere(center, radius);

				getline(file, line);
				values = split(line, ' ');
				s->setColor(values[0], values[1], values[2]);

				getline(file, line);
				values = split(line, ' ');
				s->setCoEfficients(values[0], values[1], values[2], values[3]);

				getline(file, line);
				values = split(line, ' ');
				s->setShine(values[0]);

				objects.push_back(s);

				getline(file, line);
			}
			else if (line == "triangle")
			{
				string line;

				getline(file, line);
				values = split(line, ' ');
				Point p1(values[0], values[1], values[2]);
				getline(file, line);
				values = split(line, ' ');
				Point p2(values[0], values[1], values[2]);
				getline(file, line);
				values = split(line, ' ');
				Point p3(values[0], values[1], values[2]);

				Object *t;
				t = new Triangle(p1, p2, p3);

				getline(file, line);
				values = split(line, ' ');
				t->setColor(values[0], values[1], values[2]);

				getline(file, line);
				values = split(line, ' ');
				t->setCoEfficients(values[0], values[1], values[2], values[3]);

				getline(file, line);
				values = split(line, ' ');
				t->setShine(values[0]);

				objects.push_back(t);

				getline(file, line);
			}
			else if (line == "general")
			{
				getline(file, line);
				values = split(line, ' ');
				vector<double> constants = values;

				getline(file, line);
				values = split(line, ' ');
				Point center(values[0], values[1], values[2]);
				double length = values[3];
				double width = values[4];
				double height = values[5];

				Object *g;
				g = new General(constants, center, length, width, height);

				getline(file, line);
				values = split(line, ' ');
				g->setColor(values[0], values[1], values[2]);

				getline(file, line);
				values = split(line, ' ');
				g->setCoEfficients(values[0], values[1], values[2], values[3]);

				getline(file, line);
				values = split(line, ' ');
				g->setShine(values[0]);

				objects.push_back(g);

				getline(file, line);
			}
		}

		getline(file, line);
		values = split(line, ' ');
		totalLights = values[0];
		cout << "lights " << totalLights << endl;

		for (int i = 0; i < totalLights; i++)
		{
			getline(file, line);
			values = split(line, ' ');
			Point position(values[0], values[1], values[2]);

			getline(file, line);
			values = split(line, ' ');

			lights.push_back(Light(position, values[0], values[1], values[2]));
		}

		Object *floor;
		floor = new Floor(1000, 20);
		floor->setColor(1, 1, 1);
		floor->setCoEfficients(0.4, 0.2, 0.1, 0.3);
		floor->setShine(5);
		objects.push_back(floor);

		file.close();

		values.clear();
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); //Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracing");

	loadData();
	init();

	glEnable(GL_DEPTH_TEST); //enable Depth Testing

	glutDisplayFunc(display); //display callback function
	glutIdleFunc(animate);	  //what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop(); //The main loop of OpenGL

	for (int i = 0; i < objects.size(); i++)
	{
		free(objects[i]);
	}
	objects.clear();
	lights.clear();

	return 0;
}
