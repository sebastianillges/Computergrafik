// Sample code for �bung 2

#include <math.h>
#include "vec.h"
#include "mat.h"

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"
using namespace std;

// system relevant global variables
// window width and height (choose an appropriate size)
const int g_iWidth  = 1080;
const int g_iHeight = 1080;

// global variable to tune the timer interval
int g_iTimerMSecs;

// some global state variables used to describe ...
double sun_x;
double sun_y;
double earth_x;
double earth_y;
double moon_x;
double moon_y;
double angle_earth_moon;
double angle_sun_earth;
double angle_incr_earth_moon;
double angle_incr_sun_earth;

double t_array[3][3];
double t2_array[3][3];
CVec3d sun;
CVec3d earth;
CVec3d moon;
CMat3d rotate_around_sun; // the homogeneous transformation matrix of earths rotation around the sun
CMat3d rotate_around_earth; // the homogeneous transformation matrix of the moons rotation around the earth AND the sun

class Point
{
public:
	Point(int x = 0, int y = 0)
	{
		this->x = x;
		this->y = y;
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

	float r, g, b;
};
void bhamLine(Point, Point, Color);
void rotate_origin(double *, double *, double);
void rotate(double, double, double*, double*, double);

// function to initialize our own variables
void init () 
{
	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	sun_x = 0;
	sun_y = 0;
	earth_x = g_iWidth / 4;
	earth_y = 0;
	moon_x = g_iWidth / (16.0/3.0);
	moon_y = 0;
	angle_earth_moon = 0;
	angle_sun_earth = 0;
	angle_incr_earth_moon = 0.01; // ?
	angle_incr_sun_earth = 0.001; // ?

	// init variables for display2
	double sunPos [3] = {sun_x, sun_y, 1.0};
	double earthPos [3] = {earth_x, earth_y, 1.0};
	double moonPos [3] = {moon_x, moon_y, 1.0};
	sun.setData(sunPos);
	earth.setData(earthPos);
	moon.setData(moonPos);
	double rot_array[3][3];
	rot_array[0][0] = cos(angle_incr_sun_earth);
	rot_array[0][1] = -1*sin(angle_incr_sun_earth);
	rot_array[0][2] = 0;
	rot_array[1][0] = sin(angle_incr_sun_earth);
	rot_array[1][1] = rot_array[0][0];
	rot_array[1][2] = 0;
	rot_array[2][0] = 0;
	rot_array[2][1] = 0;
	rot_array[2][2] = 1;
	rotate_around_sun = CMatrix<double,3>(rot_array);

	double rot1_array[3][3];
	rot1_array[0][0] = cos(angle_incr_earth_moon);
	rot1_array[0][1] = -1*sin(angle_incr_earth_moon);
	rot1_array[0][2] = 0;
	rot1_array[1][0] = sin(angle_incr_earth_moon);
	rot1_array[1][1] = rot1_array[0][0];
	rot1_array[1][2] = 0;
	rot1_array[2][0] = 0;
	rot1_array[2][1] = 0;
	rot1_array[2][2] = 1;
	rotate_around_earth = CMat3d(rot1_array);
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
	rotate(earth_x, earth_y, &moon_x, &moon_y, angle_incr_earth_moon);
	rotate_origin(&earth_x, &earth_y, angle_incr_sun_earth);
	rotate_origin(&moon_x, &moon_y, angle_incr_sun_earth);

	// variables for display2 ...
	t_array[0][0] = 1;
	t_array[0][1] = 0;
	t_array[0][2] = -1*earth.get(0);
	t_array[1][0] = 0;
	t_array[1][1] = 1;
	t_array[1][2] = -1*earth.get(1);
	t_array[2][0] = 0;
	t_array[2][1] = 0;
	t_array[2][2] = 1;
	t2_array[0][0] = 1;
	t2_array[0][1] = 0;
	t2_array[0][2] = earth.get(0);
	t2_array[1][0] = 0;
	t2_array[1][1] = 1;
	t2_array[1][2] = earth.get(1);
	t2_array[2][0] = 0;
	t2_array[2][1] = 0;
	t2_array[2][2] = 1;
	moon = rotate_around_sun * CMat3d(t2_array) * rotate_around_earth * CMat3d(t_array) * moon;
	earth = rotate_around_sun * earth;

	// the last two lines should always be
	glutPostRedisplay ();
	glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) 
{
	glClear (GL_COLOR_BUFFER_BIT);
	bhamLine(Point((int)sun_x, (int)sun_y), Point((int)earth_x, (int)earth_y), Color(1,0,0));
	bhamLine(Point((int)earth_x, (int) earth_y), Point((int) moon_x, (int) moon_y), Color(0,0,1));

	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

// display callback function
void display2 (void) 
{
	glClear (GL_COLOR_BUFFER_BIT);

	bhamLine(Point((int) sun.get(0), (int) sun.get(1)), Point((int) earth.get(0), (int) earth.get(1)), Color(1,0,0));
	bhamLine(Point((int) earth.get(0), (int) earth.get(1)), Point((int) moon.get(0), (int) moon.get(1)), Color(0,1,0));

	// In double buffer mode the last
	// two lines should alsways be
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

void bhamLine(Point p1, Point p2, Color c)
{
	glBegin(GL_POINTS);
		glColor3f (c.r,c.g,c.b);
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
