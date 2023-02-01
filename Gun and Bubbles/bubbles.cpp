#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

#define NUM_BUBBLES 5

int drawaxes;

int numBubbles;
int boundarySideHalf;
int innerCircleRadius;
int bubbleRadius;

double speedInc;
double speed;
double storeSpeed;

int timer;

struct point
{
	double x,y,z;
};

struct point origin;

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

double dot(struct point p1, struct point p2) {
    return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
};

void printPoint(struct point p) {
    printf("%lf %lf %lf\n", p.x, p.y, p.z);
}

double distance(struct point p1, struct point p2) {
    return sqrt(pow((p1.x-p2.x), 2) + pow((p1.y-p2.y), 2) + pow((p1.z-p2.z), 2));
}


struct bubble
{
    struct point position;
    struct point direction;
    bool inCircle;
};

struct bubble bubbles[NUM_BUBBLES];


void initializeBubbles() {
    for(int i=0; i<NUM_BUBBLES; i++) {
        bubbles[i].position.x = -boundarySideHalf + bubbleRadius;
        bubbles[i].position.y = -boundarySideHalf + bubbleRadius;
        bubbles[i].position.z = 0;

        bubbles[i].direction.x = (double)(rand()%100)/100.0;
        bubbles[i].direction.y = sqrt(1 - pow(bubbles[i].direction.x, 2));
        bubbles[i].direction.z = 0;

        bubbles[i].inCircle = false;

        printf("Direction: ");
        printPoint(bubbles[i].direction);
    }
}


bool enterInnerCircle(struct point position) {
    if(distance(origin, position)<innerCircleRadius-bubbleRadius) {
        return true;
    }
    return false;
}

bool touchBubble(struct point p1, struct point p2) {
    if(distance(p1, p2)<=2*bubbleRadius) {
        return true;
    }
    return false;
}

bool touchInnerCircle(struct point position) {
    if(distance(origin, position)>=innerCircleRadius-bubbleRadius) {
        return true;
    }
    return false;
}

char touchOuterSquare(struct point position) {
    if(position.x+bubbleRadius>=boundarySideHalf || position.x-bubbleRadius<=-boundarySideHalf) {
        return 'x';
    }
    if(position.y+bubbleRadius>=boundarySideHalf || position.y-bubbleRadius<=-boundarySideHalf) {
        return 'y';
    }
}



void moveBubbles() {
    for(int i=0; i<numBubbles; i++) {
        bubbles[i].position = add(bubbles[i].position, mult(bubbles[i].direction, speed));

        if(bubbles[i].inCircle) {
            if(touchInnerCircle(bubbles[i].position)) {
                struct point n = mult(bubbles[i].position, -1);
                double magnitude = sqrt(pow(n.x, 2) + pow(n.y, 2));
                n.x = n.x / magnitude;
                n.y = n.y / magnitude;

                double dotProduct = dot(bubbles[i].direction, n);

                if(dotProduct<0) {
                    bubbles[i].direction = add(bubbles[i].direction, mult(mult(n, 2*dot(bubbles[i].direction, n)), -1));
                }
            }

            for(int j=0; j<numBubbles; j++) {
                if(i==j) continue;
                if(!bubbles[j].inCircle) continue;
                if(touchBubble(bubbles[i].position, bubbles[j].position)) {
                    struct point n = add(bubbles[i].position, mult(bubbles[j].position, -1));
                    double magnitude = sqrt(pow(n.x, 2) + pow(n.y, 2));
                    n.x = n.x / magnitude;
                    n.y = n.y / magnitude;

                    double dotProduct = dot(bubbles[i].direction, n);
                    if(dotProduct<=0) {
                        bubbles[i].direction = add(bubbles[i].direction, mult(mult(n, 2*dot(bubbles[i].direction, n)), -1));
                        if(dotProduct>0) printf("%lf\n", dotProduct);
                    }

                    n = mult(n, -1);
                    dotProduct = dot(bubbles[j].direction, n);
                    if(dotProduct<=0) {
                        bubbles[j].direction = add(bubbles[j].direction, mult(mult(n, 2*dot(bubbles[j].direction, n)), -1));
                        if(dotProduct>0) printf("%lf\n", dotProduct);
                    }
                }
            }

            continue;
        }

        if(enterInnerCircle(bubbles[i].position)) {
            bubbles[i].inCircle = true;
            continue;
        }

        switch(touchOuterSquare(bubbles[i].position)) {
            case 'x':
                bubbles[i].direction.x *= -1;
                break;
            case 'y':
                bubbles[i].direction.y *= -1;
                break;
        }
    }
}




void drawCircle(int radius,int segments)
{
    int i;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawOuterSquare(int a) {
    glColor3f(0,1,0);
    glBegin(GL_LINE_LOOP);
    {
		glVertex3d( a, a,0);
		glVertex3d( a,-a,0);
		glVertex3d(-a,-a,0);
		glVertex3d(-a, a,0);
	}
	glEnd();
}

void drawBubbles() {
    for(int i=0; i<numBubbles; i++) {
        glPushMatrix();
        {
            glTranslatef(bubbles[i].position.x, bubbles[i].position.y, bubbles[i].position.z);
            glColor3f(0,0,1);
            drawCircle(bubbleRadius,25);
        }
        glPopMatrix();
    }
}



void keyboardListener(unsigned char key, int x,int y){
	switch(key){
		case 'p':
		    if(speed>0) {
                storeSpeed = speed;
                speed = 0;
		    } else {
		        speed = storeSpeed;
		    }
			break;
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			if((int)(speed*1000)>5) {
                speed -= speedInc;
			}
			break;
		case GLUT_KEY_UP:		// up arrow key
		    if((int)(speed*100)<50) {
                speed += speedInc;
		    }
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
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
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(0,0,200,	0,0,0,	0,1,0);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	drawOuterSquare(boundarySideHalf);

    glColor3f(1,0,0);
	drawCircle(innerCircleRadius,50);

    drawBubbles();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	//codes for any changes in Models, Camera
	if(timer>0) {
        timer++;
        if(timer%1000==0) {
            numBubbles++;
        }
        if(numBubbles==NUM_BUBBLES) {
            timer=0;
        }
	}

	moveBubbles();

	glutPostRedisplay();
}


void init(){
	//codes for initialization
	drawaxes=0;

	numBubbles=1;

    origin.x = 0;
    origin.y = 0;
    origin.x = 0;

	boundarySideHalf=100;
	innerCircleRadius=60;
	bubbleRadius=8;

	speed = 0.01;
	speedInc = 0.005;

	timer=1;

	//clear the screen
	glClearColor(0,0,0,0);

	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);

	initializeBubbles();
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

    srand(time(0));
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
