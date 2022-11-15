// Sample code for �bung 2

#include <math.h>
#include "vec.h"
#include "mat.h"

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"

void rotate_origin(double *, double *, double);
void rotate(double, double, double*, double*, double);
////////////////////////////////////////////////////////////
//
// system relevant global variables
//

// window width and height (choose an appropriate size)
const int g_iWidth  = 400;
const int g_iHeight = 400;

// global variable to tune the timer interval
int g_iTimerMSecs;

//
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// private, global variables ... replace by your own ones
//
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

CVec3d sun;
CVec3d earth;
CVec3d moon;
CMat3d rotate_earth_sun; // the homogeneous transformation matrix of earths rotation around the sun
CMat3d rotate_moon_earth_sun; // the homogeneous transformation matrix of the moons rotation around the earth AND the sun

float g_iPos;		// ... position and ...
float g_iPosIncr;	// ... position increment (used in display1)

CVec2i g_vecPos;		// same as above but in vector form ...
CVec2i g_vecPosIncr;	// (used in display2)
//
/////////////////////////////////////////////////////////////

void setpoint(int x, int y) {
	glBegin(GL_QUADS);
		glColor3f (1,0,0);
		glVertex2i (x+10, y+10);
		glColor3f (0,1,0);
		glVertex2i (x-10, y+10);
		glColor3f (0,0,1);
		glVertex2i (x+10, y-10);
		glColor3f (1,1,0);
		glVertex2i (x-10, y-10);
	glEnd();
}

// function to initialize our own variables
void init () 
{

	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	g_iPos     = 0;
	g_iPosIncr = 2;

	// init variables for display2
	int aiPos    [2] = {0, 0};
	int aiPosIncr[2] = {2, 2};
	g_vecPos.setData (aiPos);
	g_vecPosIncr.setData (aiPosIncr);

	sun_x = 0;
	sun_y = 0;
	earth_x = g_iWidth / 4;
	earth_y = 100;
	moon_x = g_iWidth / (8.0/3.0);
	moon_y = 0;
	angle_earth_moon = 0;
	angle_sun_earth = 0;
	angle_incr_earth_moon = 0.1; // ?
	angle_incr_sun_earth = 0.1; // ?

	double sunPos [2] = {sun_x, sun_y};
	double earthPos [2] = {earth_x, earth_y};
	double moonPos [2] = {moon_x, moon_y};
	sun.setData(sunPos);
	earth.setData(earthPos);
	moon.setData(moonPos);

	// TODO: Calculate the transformation matrices using translations and rotations given the positions and angular increments.
	// rotate_earth_sun.setData(?)
	// rotate_moon_earth.setData(?)
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
void timer (int value) 
{
	
	// update your variables here ...
	//

	int size2 = min (g_iWidth, g_iHeight) / 2;

	// variables for display1 ...
	if (g_iPos<=-size2 || g_iPos>=size2) g_iPosIncr = -g_iPosIncr;
	g_iPos += g_iPosIncr;

	// variables for display2 ...
	if (g_vecPos(1)<=-size2 || g_vecPos(1)>=size2) g_vecPosIncr = -g_vecPosIncr; 
	g_vecPos += g_vecPosIncr;

	//
	///////
	rotate(earth_x, earth_y, &moon_x, &moon_y, angle_incr_earth_moon);
	//rotate_origin(&earth_x, &earth_y, angle_incr_sun_earth);
	//rotate_origin(&moon_x, &moon_y, angle_incr_sun_earth);
	

	// the last two lines should always be
	glutPostRedisplay ();
	glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) 
{

	glClear (GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//

	glBegin (GL_TRIANGLES);
		glColor3f (1,0,0);
		glVertex2i ((int) sun_x, (int) sun_y);
		glColor3f (0,1,0);
		glVertex2i ((int) earth_x, (int) earth_y);
		glColor3f (0,0,1);
		glVertex2i ((int) moon_x, (int) moon_y);
	glEnd ();
	//setpoint((int) earth_x, (int) earth_y);
	//setpoint((int) moon_x, (int) moon_y);
	/*	glVertex2i ((int) moon_x+10, (int) moon_y+10);
		glColor3f (0,1,0);
		glVertex2i ((int) moon_x-10, (int) moon_y+10);
		glColor3f (0,0,1);
		glVertex2i ((int) moon_x+10, (int) moon_y-10);
		glColor3f (1,1,0);
		glVertex2i ((int) moon_x-10, (int) moon_y-10);
	*/
	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

// display callback function
void display2 (void) 
{
	glClear (GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//

	glBegin (GL_QUADS);
		glColor3f (1,0,0);
		glVertex2i (-g_vecPos(1), -g_vecPos(2));
		glColor3f (0,1,0);
		glVertex2i (g_vecPos(1), -g_vecPos(2));
		glColor3f (0,0,1);
		glVertex2i (g_vecPos(1), g_vecPos(2));
		glColor3f (1,1,0);
		glVertex2i (-g_vecPos(1), g_vecPos(2));
	glEnd ();

	//
	///////

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

/*
void bhamLine (Point p1, Point p2, Color c) 
{
	glBegin (GL_POINTS);
	glColor3f (c.r, c.g, c.b);
	// ...

		// implement bhamLine here and use
		// glVertex2i (x, y);
		// to draw a pixel
	
	// ...
	glEnd ();
}
*/
void rotate_origin(double * px, double * py, double increment) {
	double distance = sqrt(pow(*px,2)*pow(*py,2));
	double angle = atan2(*py,*px) + increment;
	*px = cos(angle) * distance;
	*py = sin(angle) * distance;
}

void rotate(double px1, double py1, double* px2, double* py2, double increment) {
	double px = *px2 - px1;
	double py = *py2 - py1;
	double distance = sqrt(pow(px,2)*pow(py,2));
	double angle = atan2(py,px) + increment;
	*px2 = cos(angle) * distance + px1;
	*py2 = sin(angle) * distance + py1;
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
