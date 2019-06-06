#include <time.h>
#include "Dot.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

using namespace std;

float red = 1.0, green = 0.0, blue = 0.0;
int tmpx, tmpy; // store the first point when shape is line, rectangle or circle
int brushSize = 4;
int eraserSize = 1;
bool isSecond = false;	//to check if it is a second point in case of line, rect or circle
bool isRandom = false;  // to generate random colours
bool isEraser = false;
float window_w = 500;
float window_h = 500;

void *font;
void *currentfont;

int shape = 1; // 1:point, 2:line, 3:rectangle, 4:circle, 5:brush

vector<Dot> dots;		// store all the points until clear

void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < dots.size(); i++)
	{
		glColor3f(dots[i].getR(), dots[i].getG(), dots[i].getB());
		glVertex2i(dots[i].getX(), dots[i].getY());
	}
	glEnd();
	glutSwapBuffers();  //swaps the buffers of current window (double buffer enabled)
}

void clear()
{
	dots.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	cout << "[Info] The window and the history are cleared successfully.\n";
}

void quit()
{
	std::cout << "Thank you for using this Paint tool! Goodbye!" << std::endl;
	exit(0);
}

void drawDot(int mousex, int mousey)
{
	Dot newDot(mousex, window_h - mousey, isEraser ? 1.0 : red, isEraser ? 1.0 : green, isEraser ? 1.0 : blue);
	dots.push_back(newDot);
}

void drawBrush(int x, int y)
{
	for (int i = 0; i < brushSize; i++)
	{
		int randX = rand() % (brushSize + 1) - brushSize / 2 + x;
		int randY = rand() % (brushSize + 1) - brushSize / 2 + y;
		drawDot(randX, randY);
	}
}

void drawLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	// Bresenham's line algorithm is only good when abs(dx) >= abs(dy)
	// So when abs(dx) < abs(dy), change axis x and y
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawDot(y, x);
	else
		drawDot(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawDot(y, x);
		else
			drawDot(x, y);
	}
}

/**
 * We can use drawLine function to draw the rectangle
 * 
 * Top-left corner specified by the first click,
 * and the bottom-right corner specified by a second click
 */
void drawRectangle(int x1, int y1, int x2, int y2)
{
	if (x1 < x2 && y1 < y2)
	{
		drawLine(x1, y1, x2, y1);		// top
		drawLine(x2, y1, x2, y2);		// right
		drawLine(x2, y2, x1, y2);		// bottom
		drawLine(x1, y2, x1, y1);		// left
	}
	else	
		std::cout 	<< "[Warning] The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	
}


//  Midpoint circle algorithm
void drawCircle(int x1, int y1, int x2, int y2)
{
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawDot(x1 + x, y1 + y);
		drawDot(x1 - x, y1 + y);
		drawDot(x1 + x, y1 - y);
		drawDot(x1 - x, y1 - y);
		drawDot(x1 + y, y1 + x);
		drawDot(x1 - y, y1 + x);
		drawDot(x1 + y, y1 - x);
		drawDot(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 5;
		}
	}
}


void erase(int x, int y)
{
	for (int i = -eraserSize; i <= eraserSize; i++)
	{
		for (int j = -eraserSize; j <= eraserSize; j++)
		{
			drawDot(x + i, y + j);
		}
	}
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: // 27 is the esc key
		quit();
		break;
	case 'c':
		clear();
		break;
	case '+':
		if (shape == 5 && !isEraser)
		{
			if (brushSize < 16)
				brushSize += 4;
			else
				std::cout	 << "[Warning] Airbrush's size cannot be larger. It is already the largest.\n";	
		}
		else if (isEraser)
		{
			if (eraserSize < 10)
				eraserSize += 4;
			else

				std::cout	 << "[Warning] Eraser's size cannot be larger. It is already the largest.\n";
		
		}
		break;
	case '-':
		if (shape == 5 && !isEraser)
		{
			if (brushSize > 4)
				brushSize -= 4;
			else
				std::cout	 << "[Warning] Airbrush's size cannot be smaller. It is already the smallest.\n";
		}
		else if (isEraser)
		{
			if (eraserSize > 2)
				eraserSize -= 4;
			else

				std::cout	 << "[Warning] Eraser's size cannot be smaller. It is already the smallest.\n";
			
		}
		break;

	}
}

void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (isRandom)
		{
			srand(time(NULL));		//sets starting range value for rand()
			red = float(rand()) / float(RAND_MAX);
			green = float(rand()) / float(RAND_MAX);
			blue = float(rand()) / float(RAND_MAX);
		}
		if (isEraser)
		{
			erase(x, y);
		}
		else
		{
			if (shape == 1)
			{
				drawDot(x, y);
			}
			else if (shape == 5)
			{
				drawBrush(x, y);
			}
			else
			{
				if (!isSecond)		// to check if it is the first point 
				{
					tmpx = x;		//storing first point's (x, y)
					tmpy = y;
					isSecond = true;
				}
				else
				{
					if (shape == 2)
						drawLine(tmpx, tmpy, x, y);
					else if (shape == 3)
						drawRectangle(tmpx, tmpy, x, y);
					else if (shape == 4)
						drawCircle(tmpx, tmpy, x, y);
					isSecond = false;
				}
			}
		}
	}
}

void motion(int x, int y)
{
	if (isEraser)
		erase(x, y);
	else
	{
		if (shape == 1)
		{
			drawDot(x, y);
		}
		if (shape == 5)
			drawBrush(x, y);
	}
}

void reshape(int w, int h)	// mains constant screen content when the window is resized
{
	window_w = w;
	window_h = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void processMainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
		break;
	case 1:
		clear();
		break;
	}
}

void processBrushSizeMenu(int value)
{
	shape = 5;
	isEraser = false;
	brushSize = value;
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

void processColourMenu(int value)
{
	isSecond = false;
	isEraser = false;
	isRandom = false;

	switch (value)
	{
	case 1: // red
		red = 1.0;
		green = 0.0;
		blue = 0.0;
		break;
	case 2: // green
		red = 0.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 3: // blue
		red = 0.0;
		green = 0.0;
		blue = 1.0;
		break;
	case 4: // purple
		red = 0.5;
		green = 0.0;
		blue = 0.5;
		break;
	case 5: // yellow
		red = 1.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 6: // random
		isRandom = true;
		break;
	}
}

void processShapeMenu(int value)  //sets shape of mouse pointer
{
	shape = value;
	isEraser = false;
	isSecond = false;

	switch (shape)
	{
	case 1:
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		break;
	case 2:
	case 3:
	case 4:
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}

void processEraserSizeMenu(int value)
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	eraserSize = value;
	isEraser = true;
}

void createOurMenu()
{
	int colourMenu = glutCreateMenu(processColourMenu); //returns unique identifier for the menu
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Purple", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Random", 6);

	int sizeMenu = glutCreateMenu(processBrushSizeMenu);
	glutAddMenuEntry("4px", 4);
	glutAddMenuEntry("8px", 8);
	glutAddMenuEntry("12px", 12);
	glutAddMenuEntry("16px", 16);

	int shapeMenu = glutCreateMenu(processShapeMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Rectangle", 3);
	glutAddMenuEntry("Circle", 4);
	glutAddSubMenu("Airbrush", sizeMenu);

	int eraserSizeMenu = glutCreateMenu(processEraserSizeMenu);
	glutAddMenuEntry("Small", 2);
	glutAddMenuEntry("Medium", 6);
	glutAddMenuEntry("Large", 10);


	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Colour", colourMenu);
	glutAddSubMenu("Shapes", shapeMenu);
	glutAddSubMenu("Eraser", eraserSizeMenu);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(void)
{
	/* background color */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(0, FPS, 0);  //To refresh the frames 
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);	// called when window size is changed
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);		// calle on mouse click
	glutMotionFunc(motion);		// called when mouse moves while mouse button is pressed
	glutTimerFunc(17, FPS, 0);
}

void printGuide()
{
	std::cout << "#########################################################################\n"
			  << "#                    Welcome to use this Paint tool!                    #\n"
			  << "#########################################################################\n"
			  << "A list of commands:\n"
			  << "Right click\t"
			  << "-> show menu\n"
			  << "Left click\t"
			  << "-> choose option\n"
			  << "Menu \"Colour\"\t"
			  << "-> You can choose Red, Green, Blue, Yellow or Random, the default color is Red.\n"
			  << "Menu \"Shapes\"\t"
			  << "-> The default shape is Point.\n"
			  << "\tPoint\t\t"
			  << "-> draw a dot at the point clicked with the mouse. Clicking and dragging will draw points constantly like free-form drawing.\n"
			  << "\tLine\t\t"
			  << "-> draw a linebetween two subseauently clicked points. (Bresenham's algorithm is used here.)\n"
			  << "\tRectangle\t"
			  << "-> draw a rectangle with top-left corner specified by the first click and the bottom-right corner specified by a second click. If the second click is bottom-left, top-right or top-left comparing to the first click, a warning will show in the console.\n"
			  << "\tCircle\t\t"
			  << "-> draw a circle centered at the position of the first click, with its radius set by a second click.\n"
			  << "\tAirbrush\t"
			  << "-> draw multiple points as brush around the clicked point. There are four options of size.\n"
			  << "Menu \"Eraser\"\t"
			  << "-> erase the points by clicking and dragging.\n"
			  << "Menu \"Clear\"\t"
			  << "-> clear all the points.\n"
			  << "Menu \"Quit\"\t"
			  << "-> close the window.\n"
			  << "Keyboard 'q'\t"
			  << "-> close the window.\n"
			  << "Keyboard 'esc'\t"
			  << "-> close the window.\n"
			  << "Keyboard 'c'\t"
			  << "-> clear all the points.\n"
			  << "Keyboard '+'\t"
			  << "-> larger size of eraser or brush.\n"
			  << "Keyboard '-'\t"
			  << "-> smaller size of eraser or brush.\n"
			  << "################################# Paint #################################" << std::endl;
}

void myInit(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.7,0.7,0.7,0.0);
    glEnable(GL_DEPTH_TEST);
    gluOrtho2D(0.0,400.0,0.0,300.0);
}

void menucallback(int option){
    switch(option){
        case 1 : glutLeaveMainLoop(); break;
		case 2 : exit(0); break;
        default : break;
    }
}

void drawstring(float x,float y,float z,char const *string)
{
    char const *c;
    glRasterPos3f(x,y,z);
    for(c=string; *c!='\0'; c++)
    {
	   glColor3f(0.0,1.0,1.0);
           glutBitmapCharacter(currentfont,*c);
    }
}

//To set the font of the string
void setFont(void *font)
{
    currentfont=font;
}

void display_about(void)		//Displaying the first page
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(150.0,275.0,0.0," CG MINI PROJECT ON");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(160,260,0.0,"DIGITAL CANVAS");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(200,200,0,"BY:");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(80,185,0,"JEHAD MOHAMED");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(260,185,0,"4SF16CS060");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(260,170,0,"4SF16CS006");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(80,170,0,"ADITHYA M SUVARNA");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(90,120,0,"SAHYADRI COLLEGE OF ENGINEERING AND MANAGEMENT");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(165,80,0,"PROJECT GUIDE:");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(150,65,0,"Ms. CHAITRA  ACHARYA");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.0,1.0,1.0);
    drawstring(160,50,0,"ASST. PROFESSOR");

    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1.0,1.0,1.0);
    drawstring(280,20,0.0,"Right Click for Menu");

    glFlush();
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS ); 
    glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,768);
    glutCreateWindow("Digital Canvas");
    glutDisplayFunc(display_about);
    glutCreateMenu(menucallback);
    glutAddMenuEntry("Go to the canvas",1);
	glutAddMenuEntry("Quit",2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myInit();
    glutMainLoop();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1366,768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paint");
	callbackInit();
	init();
	printGuide();
	createOurMenu();
	glutMainLoop();
	return (0);
}