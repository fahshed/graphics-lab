#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))


int drawgrid;
int drawaxes;

double translationInc;
double rotationInc;
double sphereRotInc;
double sphereZRotAngle;
double sphereHalfRotAngle;
double cylinderRotAngle;
double cylinderSpinAngle;

struct point {
	double x,y,z;
};

struct point add(struct point p1, struct point p2) {
    struct point result;

    result.x = p1.x + p2.x;
    result.y = p1.y + p2.y;
    result.z = p1.z + p2.z;

    return result;
};

struct point mult(struct point p, double value) {
    struct point result;

    result.x = p.x*value;
    result.y = p.y*value;
    result.z = p.z*value;

    return result;
};

struct point crossProduct(struct point p1, struct point p2) {
    struct point product;

    product.x = p1.y*p2.z - p1.z*p2.y;
    product.y = p1.z*p2.x - p1.x*p2.z;
    product.z = p1.x*p2.y - p1.y*p2.x;

    return product;
};

void printPoint(struct point p) {
    printf("%lf %lf %lf\n", p.x, p.y, p.z);
}


struct point x, y, z, u, r, l, pos, target;

void initializeAxes() {
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

void initializeCamera() {
    u.x = 0;
    u.y = 0;
    u.z = 1;

    r.x = -1/sqrt(2);
    r.y = 1/sqrt(2);
    r.z = 0;

    l.x = -1/sqrt(2);
    l.y = -1/sqrt(2);
    l.z = 0;

    pos.x = 170;
    pos.y = 170;
    pos.z = 40;

    printf("u: %lf %lf %lf\n", u.x, u.y, u.z);
    printf("r: %lf %lf %lf\n", r.x, r.y, r.z);
    printf("l: %lf %lf %lf\n", l.x, l.y, l.z);
}

void initializeGun() {
    target.x = 0;
    target.y = 1;
    target.z = 0;
}



void drawTargetAxis() {
    glBegin(GL_LINES);
    {
        glColor3f(1, 1, 0);
        glVertex3f( 0,0,0);
        glVertex3f(200*target.x,200*target.y,200*target.z);
    }
    glEnd();
}

void drawAxes()
{
	if(drawaxes==1)
	{
		//glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
		    glColor3f(1, 0, 0);
			glVertex3f( 150,0,0);
			glVertex3f(-150,0,0);

            glColor3f(0, 1, 0);
			glVertex3f(0,-150,0);
			glVertex3f(0, 150,0);

			glColor3f(0, 0, 1);
			glVertex3f(0,0, 150);
			glVertex3f(0,0,-150);
		}glEnd();
	}
}

void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}

void drawHemesphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
            glColor3f(j%2,j%2,j%2);
			glBegin(GL_QUADS);
			{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}
			glEnd();
		}
	}
}

void drawReversephere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=2*radius - radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=-h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
            glColor3f(j%2,j%2,j%2);
			glBegin(GL_QUADS);
			{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}
			glEnd();
		}
	}
}

void drawCylinder(double radius, double height, int slices) {
    struct point points[100][100];
	int i,j;
	double h,r;
	int stacks = 1;
	r = radius;
	h = height;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=-i*h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
            glColor3f(j%2,j%2,j%2);
			glBegin(GL_QUADS);
			{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}
			glEnd();
		}
	}
}


void drawGun(double radius,int slices,int stacks)
{
    glRotatef(90, x.x,x.y,x.z);

	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}

	glRotatef(sphereZRotAngle, y.x,y.y,y.z);

	//upper hemisphere
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);
			{
			    glColor3f(j%2,j%2,j%2);
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}
			glEnd();
		}
	}

	glRotatef(sphereHalfRotAngle, x.x,x.y,x.z);

    //lower hemisphere
    for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);
			{
			    glColor3f(j%2,j%2,j%2);
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}
			glEnd();
		}
	}

    glTranslatef(0,0,-radius);

    glRotatef(cylinderRotAngle, x.x,x.y,x.z);

    glRotatef(cylinderSpinAngle, z.x,z.y,z.z);

    glTranslatef(0,0,-radius/2);

    drawHemesphere(radius/2, slices, stacks);

    drawCylinder(radius/2, 2*radius, slices);

    glTranslatef(0,0,-(2*radius));

    drawReversephere(radius/2, slices, stacks);
}


void moveForward() {
    pos = add(pos, mult(l, translationInc));
}

void moveBackward() {
    pos = add(pos, mult(l, -translationInc));
}

void moveRight() {
    pos = add(pos, mult(r, translationInc));
}

void moveLeft() {
    pos = add(pos, mult(r, -translationInc));
}

void moveUp() {
    pos = add(pos, mult(u, translationInc));
}

void moveDown() {
    pos = add(pos, mult(u, -translationInc));
}


struct point rotateArround(struct point k, struct point v, double theta) {
    struct point cross = crossProduct(k, v);
    return add(mult(v, cos(theta)), mult(cross, sin(theta)));
};


void rotateLeft() {
    l = rotateArround(u, l, rotationInc);
    r = rotateArround(u, r, rotationInc);
}

void rotateRight() {
    l = rotateArround(u, l, -rotationInc);
    r = rotateArround(u, r, -rotationInc);
}

void lookUp() {
    l = rotateArround(r, l, rotationInc);
    u = rotateArround(r, u, rotationInc);
}

void lookDown() {
    l = rotateArround(r, l, -rotationInc);
    u = rotateArround(r, u, -rotationInc);
}

void tiltLeft() {
    u = rotateArround(l, u, rotationInc);
    r = rotateArround(l, r, rotationInc);
}

void tiltRight() {
    u = rotateArround(l, u, -rotationInc);
    r = rotateArround(l, r, -rotationInc);
}


void keyboardListener(unsigned char key, int xx,int yy){
	switch(key){
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
        case 'q':
            if(sphereZRotAngle<45) {
                sphereZRotAngle+=sphereRotInc;
                //target = rotateArround(z, target, sphereRotInc*pi/180);
            }
			break;
        case 'w':
            if(sphereZRotAngle>-45) {
                sphereZRotAngle-=sphereRotInc;
                //target = rotateArround(z, target, -sphereRotInc*pi/180);
            }
			break;
        case 'e':
            if(sphereHalfRotAngle<45) {
                sphereHalfRotAngle+=sphereRotInc;
                //target = rotateArround(x, target, sphereRotInc*pi/180);
            }
			break;
        case 'r':
            if(sphereHalfRotAngle>-45) {
                sphereHalfRotAngle-=sphereRotInc;
                //target = rotateArround(x, target, -sphereRotInc*pi/180);
            }
			break;
        case 'a':
            if(cylinderRotAngle<45) {
                cylinderRotAngle+=sphereRotInc;
                //target = rotateArround(x, target, sphereRotInc*pi/180);
            }
			break;
        case 's':
            if(cylinderRotAngle>-45) {
                cylinderRotAngle-=sphereRotInc;
                //target = rotateArround(x, target, -sphereRotInc*pi/180);
            }
			break;
        case 'd':
            if(cylinderSpinAngle>-45) {
                cylinderSpinAngle-=sphereRotInc;
            }
			break;
        case 'f':
            if(cylinderSpinAngle<45) {
                cylinderSpinAngle+=sphereRotInc;
            }
			break;
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_UP:
		    moveForward();		// up arrow key
			//cameraHeight += 3.0;
			break;
        case GLUT_KEY_DOWN:		//down arrow key
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

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			break;

		case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void display(){
	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);
	//initialize the matrix
	glLoadIdentity();
	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?
    gluLookAt(pos.x,pos.y,pos.z,    pos.x+l.x,pos.y+l.y,pos.z+l.z,  u.x,u.y,u.z);
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

    drawAxes();
	//drawGrid();
    //drawTargetAxis();
	drawGun(30,70,25);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;

	translationInc=5;
	rotationInc=0.05;
    sphereRotInc=2.5;

	sphereZRotAngle=0;
	sphereHalfRotAngle=0;
	cylinderRotAngle=0;
	cylinderSpinAngle=0;

	//clear the screen
	glClearColor(0,0,0,0);
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	initializeAxes();
	initializeCamera();
	initializeGun();
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
