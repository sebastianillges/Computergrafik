// Sample code for �bung 2

#include <math.h>
#include "vec.h"
#include "mat.h"

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"
#include <iostream>
using namespace std;
#include <map>

// system relevant global variables
// window width and height (choose an appropriate size)
const int g_iWidth  = 2560;
const int g_iHeight = 1600;

// global variable to tune the timer interval
int g_iTimerMSecs;

class Point
{
public:
	Point(int x = 0, int y = 0)
	{
		this->x = x;
		this->y = y;
	}
	bool operator() (const Point& lhs, const Point& rhs) const
   	{
       return lhs.x < rhs.x;
   	}
	int x, y;
};
class Color
{
public:
	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	bool equals(Color c) {
		return (this->r == c.r && this->g == c.g && this->b == c.b);
	}
	float r, g, b;
};
map<int, Color> colormap;
void bhamLine(Point, Point, Color);
void aliasLine(Point, Point, Color);
void bhamCircle(Point, int, Color);
void rotate_origin(double *, double *, double);
void rotate(double, double, double*, double*, double);
CMat3d rotate_matrix(double, double, double, double, double);
void floodFill(Point, Color);

// function to initialize our own variables
void init () 
{
	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1


	// init variables for display2
	
}

// function to initialize the view to ortho-projection
void initGL () 
{
	glViewport (0, 0, g_iWidth, g_iHeight);	// Establish viewing area to cover entire window.

	glMatrixMode (GL_PROJECTION);			// Start modifying the projection matrix.
	glLoadIdentity ();						// Reset project matrix.
	glOrtho (-g_iWidth/2, g_iWidth/2, -g_iHeight/2, g_iHeight/2, 0, 1);	// Map abstract coords directly to window coords.

	// tell GL that we draw to the back buffer and
	// swap buffers when image is ready to avoid flickering
	glDrawBuffer (GL_BACK);

	// tell which color to use to clear image
	glClearColor (0,0,0,1);
}

int min (int a, int b) { return a>b? a: b; }
// timer callback function
void timer (int value) {
	// variables for display1 ...

	// variables for display2 ...

	// the last two lines should always be
	glutPostRedisplay ();
	glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) 
{
	glClear (GL_COLOR_BUFFER_BIT);
	
	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

// display callback function
void display2 (void) 
{
	glClear (GL_COLOR_BUFFER_BIT);

	// In double buffer mode the last
	// two lines should always be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) 
{
	switch (key) {
		case 'q':
		case 'Q':
			exit (0); // quit program
			break;
		case '1':
			glutDisplayFunc (display1);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		case '2':
			glutDisplayFunc (display2);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		default:
			// do nothing ...
			break;
	};
}

void rotate_origin(double * px, double * py, double increment) {
	double distance = sqrt(pow(*px,2)+pow(*py,2));
	double angle = atan2(*py,*px) + increment;
	*px = cos(angle) * distance;
	*py = sin(angle) * distance;
}

void rotate(double px1, double py1, double* px2, double* py2, double increment) {
	double px = *px2 - px1;
	double py = *py2 - py1;
	double distance = sqrt(pow(px,2)+pow(py,2));
	double angle = atan2(py,px) + increment;
	*px2 = cos(angle) * distance + px1;
	*py2 = sin(angle) * distance + py1;
}

CMat3d rotate_matrix(double px1, double py1, double px2, double py2, double angle) {
	double rot_array[3][3];
	rot_array[0][0] = cos(angle);
	rot_array[0][1] = -1*sin(angle);
	rot_array[0][2] = 0;
	rot_array[1][0] = sin(angle);
	rot_array[1][1] = rot_array[0][0];
	rot_array[1][2] = 0;
	rot_array[2][0] = 0;
	rot_array[2][1] = 0;
	rot_array[2][2] = 1;
	CMat3d rotate = CMatrix<double,3>(rot_array);
	if (px1 == 0 && py1 == 0) {
		return rotate;
	}
	double t_array[3][3];
	double t2_array[3][3];
	t_array[0][0] = 1;
	t_array[0][1] = 0;
	t_array[0][2] = -px1;
	t_array[1][0] = 0;
	t_array[1][1] = 1;
	t_array[1][2] = -py1;
	t_array[2][0] = 0;
	t_array[2][1] = 0;
	t_array[2][2] = 1;
	t2_array[0][0] = 1;
	t2_array[0][1] = 0;
	t2_array[0][2] = px1;
	t2_array[1][0] = 0;
	t2_array[1][1] = 1;
	t2_array[1][2] = py1;
	t2_array[2][0] = 0;
	t2_array[2][1] = 0;
	t2_array[2][2] = 1;
	return CMat3d(t2_array)*rotate*CMat3d(t_array);
}

int abs(int a)
{
    return (((a) < 0) ? -(a) : a);
}
// Xiaolin Wu , Line Algorithm
void aliasLine(Point p1, Point p2, Color c)
{
    glBegin(GL_POINTS);
    glColor3f(c.r, c.g, c.b);
    glVertex2i(p1.x, p1.y);

    int dx = p2.x - p1.x;
    int ax = abs(dx) << 1;
    int sx = (((dx) < 0) ? -1 : 1);

    int dy = p2.y - p1.y;
    int ay = abs(dy) << 1;
    int sy = (((dy) < 0) ? -1 : 1);

    if (ax > ay)
    {

        int d = ay - (ax >> 1);
        while (p1.x != p2.x)
        {
            glColor3f(c.r, c.g, c.b);
            glVertex2i(p1.x, p1.y);

            if (d >= 0)
            {
                p1.y += sy;
                d -= ax;
            }
            p1.x += sx;
            d += ay;
        }
    }
    else
    {

        int d = ax - (ay >> 1);
        while (p1.y != p2.y)
        {
            glColor3f(c.r, c.g, c.b);
            glVertex2i(p1.x, p1.y);
            if (d >= 0)
            {
                p1.x += sx;
                d -= ay;
            }
            p1.y += sy;
            d += ax;
        }
    }

    glColor3f(c.r, c.g, c.b);
    glVertex2i(p1.x, p1.y);

    glEnd();
}

void bhamLine(Point p1, Point p2, Color c)
{
	glBegin(GL_POINTS);
		glColor3f(c.r,c.g,c.b);
		glVertex2i(p1.x, p1.y);
		int delta_X = p2.x - p1.x;
		int delta_Y = p2.y - p1.y;
		int *fast, *slow, delta_NE, delta_E, d, *sFast, *sSlow;
		// Richtungs bestimmen
		int sx = delta_X >= 0 ? 1 : -1; // links rechts
		int sy = delta_Y >= 0 ? 1 : -1; // hoch runter
		if (abs(delta_X) >= abs(delta_Y))
		{
			fast = &p1.x; // x kann 2 pixel
			slow = &p1.y; // y kann 1 pixel
			sFast = &sx;
			sSlow = &sy;
			delta_NE = (abs(delta_Y) - abs(delta_X)) * 2;
			delta_E = abs(2 * delta_Y);
			//		langsame richtung - schnelle richtung
			d = 2 * abs(delta_Y) - abs(delta_X);
		}
		else
		{
			fast = &p1.y;
			slow = &p1.x;
			sFast = &sy;
			sSlow = &sx;
			delta_NE = (abs(delta_X) - abs(delta_Y)) * 2;
			delta_E = abs(2 * delta_X);
			d = 2 * abs(delta_X) - abs(delta_Y);
		}

		while (p1.x != p2.x || p1.y != p2.y)
		{
			if (d >= 0)
			{
				d += delta_NE;
				*fast += *sFast;
				*slow += *sSlow;
			}
			else
			{
				d += delta_E;
				*fast += *sFast;
			}
			glColor3f (c.r,c.g,c.b);
			glVertex2i(p1.x, p1.y);
		}
	glEnd();
}

void bhamCircle(Point p, int r, Color c) {
  // Mittelpunkt
	glBegin(GL_POINTS);
		glColor3f(c.r,c.g,c.b);
		glVertex2i(p.x, p.y);
		//colormap.insert({p.x*g_iHeight+p.y, c});
    glEnd();

		int x, y, d, dSE, dE, trueX, trueY;

		trueX = p.x;
		trueY = p.y;
		x = 0;
		y = r;
		d = 5 - 4 * r;
    glBegin(GL_LINES);
    glVertex2i(x+trueX, y+trueY);
		glVertex2i(y+trueX, x+trueY);
    glEnd();
    glBegin(GL_LINES);
    glVertex2i(x+trueX, y+trueY);
    glVertex2i(-y+trueX, -x+trueY);
    glEnd();

		while (y > x) {
			if (d >= 0) {
				dSE =  4 * (2 * (x - y) + 5);
				d += dSE;
				x++;
				y--;
			}
			else {
				dE = 4 * (2 * x + 3);
				d += dE;
				x++;
			}
      glBegin(GL_LINES);
			glVertex2i(-y+trueX, x+trueY);
			glVertex2i(y+trueX, -x+trueY);
      glEnd();
      glBegin(GL_LINES);
			glVertex2i(-x+trueX, y+trueY);
			glVertex2i(x+trueX, -y+trueY);
      glEnd();
      glBegin(GL_LINES);
        glVertex2i(y+trueX, x+trueY);
        glVertex2i(-y+trueX, -x+trueY);
      glEnd();
      glBegin(GL_LINES);
        glVertex2i(x+trueX, y+trueY);
        glVertex2i(-x+trueX, -y+trueY);
      glEnd();
		}
	//glEnd();
}

int main (int argc, char **argv) 
{
	glutInit (&argc, argv);
	// we have to use double buffer to avoid flickering
	// TODO: lookup "double buffer", what is it for, how is it used ...
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutCreateWindow ("Übung 2");

	init ();	// init my variables first
	initGL ();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);
	glutDisplayFunc (display1);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}
