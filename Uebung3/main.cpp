// Sample code for �bung 2

#include <math.h>
#include "vec.h"
#include "mat.h"

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"
#include <iostream>
using namespace std;

// system relevant global variables
// window width and height (choose an appropriate size)
const int g_iWidth  = 1080;
const int g_iHeight = 1080;

// global variable to tune the timer interval
int g_iTimerMSecs;

// cuboids
CVec4f c1[8];
CVec4f c2[8];
CVec4f c3[8];

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
void bhamCircle(Point p, int r, Color c)
{
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

CVec4f projectZ(float fFocus, CVec4f pView)
{
	CVec3f viewOrigin;
	CVec3f viewDir;
	CVec3f viewUp;
	CVec3f viewLeft;
	float o_arr[3] = {0, 0, 0};
	float d_arr[3] = {0, 0, -1};
	float u_arr[3] = {0, 1, 0};
	float l_arr[3] = {1, 0, 0};
	viewOrigin.setData(o_arr);
	viewDir.setData(d_arr);
	viewUp.setData(u_arr);
	viewLeft.setData(l_arr);

	CVec3f eyePoint;
	float e_arr[3] = {0, 0, fFocus};
	eyePoint.setData(e_arr);

	
	float m_mat[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 1/-fFocus, 1}};
	CMat4f m = CMatrix<float,4>(m_mat);

	//return pView * m;
	float x = pView.get(0);
	float y = pView.get(1);
	float z = pView.get(2);
	float k = fFocus / (fFocus - z);

	float xn = x * k;
	float yn = y * k;

	CVec4f pNew;
	float pNewArr[4] = {xn, yn, 0, 1};
	pNew.setData(pNewArr);
	return pNew;

}

void drawProjektedZ(CVec4f points[8], Color c) {
	Point Points[8];
	for (int i = 0; i < 8; i++) {
		Points[i] = Point((int) points[i].get(0), (int) points[i].get(1));
	}
	bhamLine(Points[0], Points[1], c);
	bhamLine(Points[0], Points[3], c);
	bhamLine(Points[0], Points[4], c);
	bhamLine(Points[1], Points[2], c);
	bhamLine(Points[1], Points[5], c);
	bhamLine(Points[2], Points[3], c);
	bhamLine(Points[2], Points[6], c);
	bhamLine(Points[3], Points[7], c);
	bhamLine(Points[4], Points[5], c);
	bhamLine(Points[4], Points[7], c);
	bhamLine(Points[5], Points[6], c);
	bhamLine(Points[6], Points[7], c);
}

//     5------6
//    /|     /|
//   1------2 |
//   | 4----|-7
//   |/     |/
//   0------3

void drawQuader(CVec4f Cuboid[8],float fFocus, Color c) {
	CVec4f points[8];
	for (int i = 0; i < 8; i++) {
		points[i] = projectZ(fFocus, Cuboid[i]);
	}
	drawProjektedZ(points, c);
}

// function to initialize our own variables
void init () 
{
	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	float c1p0[4] = {-100, -100, 100, 1};
	float c1p1[4] = {-100, 300, 100, 1};
	float c1p2[4] = {300, 300, 100, 1};
	float c1p3[4] = {300, -100, 100, 1};
	float c1p4[4] = {-100, -100, 500, 1};
	float c1p5[4] = {-100, 300, 500, 1};
	float c1p6[4] = {300, 300, 500, 1};
	float c1p7[4] = {300, -100, 500, 1};
	
	c1[0].setData(c1p0);
	c1[1].setData(c1p1);
	c1[2].setData(c1p2);
	c1[3].setData(c1p3);
	c1[4].setData(c1p4);
	c1[5].setData(c1p5);
	c1[6].setData(c1p6);
	c1[7].setData(c1p7);

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
	drawQuader(c1, -1000, Color(1,0,0));
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

void resize(int width, int height) {
    // we ignore the params and do:
    glutReshapeWindow(1080, 1080);
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
	//glutReshapeFunc(resize);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}
