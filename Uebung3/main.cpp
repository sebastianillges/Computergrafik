#include <math.h>
#include "vec.h"
#include "mat.h"
#include "cuboid.h"
#include "glut.h"
#include "utility.h"
#include <iostream>
using namespace std;

// system relevant global variables
// window width and height (choose an appropriate size)
const int g_iWidth  = 1080;
const int g_iHeight = 1080;

// global variable to tune the timer interval
int g_iTimerMSecs;

// cuboids
Cuboid cuboid1;
Cuboid cuboid2;
Cuboid cuboid3;

// view system
CVec4f viewOrigin;
CVec4f viewDir;
CVec4f viewUp;
CVec4f viewLeft;

// Clipping 
#define CLIPLEFT  1  // Binär   0001
#define CLIPRIGHT 2  //         0010
#define CLIPLOWER 4  //         0100
#define CLIPUPPER 8  //         1000

float xMin = -g_iWidth / 2;
float xMax = (g_iWidth / 2) - 1;
float yMin = -g_iHeight / 2;
float yMax = (g_iHeight / 2) - 1;

CMat4f base_change_mat;
float fFocus;
int stride;
float pi = atan(1) * 4;
float angle_deg;
float angle_rad;

void clipLine(Point &p1, Point &p2) {
	int delta_X = p2.x - p1.x;
	int delta_Y = p2.y - p1.y;
	if (!(p2.x == p1.x || p2.y == p1.y)) {
	printf("a\n");
	// Clipping
	int K1=0,K2=0;
	if(p1.y < yMin) K1 =CLIPLOWER;
	if(p1.y > yMax) K1 =CLIPUPPER;
	if(p1.x < xMin) K1|=CLIPLEFT;
	if(p1.x > xMax) K1|=CLIPRIGHT;

	if(p2.y < yMin) K2 =CLIPLOWER;
	if(p2.y > yMax) K2 =CLIPUPPER;
	if(p2.x < xMin) K2|=CLIPLEFT;
	if(p2.x > xMax) K2|=CLIPRIGHT;

	int break_while = 0;

	while ( K1 || K2 )
	{	
		printf("%d\n", break_while);
		printf("%d, %d\n", delta_X, delta_Y);
		if (break_while > 5) {
			break;
		}
		if( K1 & K2 ) break;											// muss nix gezeichnet werden
		
		if ( K1 ) {
			if( K1 & CLIPLEFT ) {
				p1.y += (xMin - p1.x) * delta_Y / delta_X;
				p1.x = xMin;
			}
			else if ( K1 & CLIPRIGHT ) {
				p1.y += (xMax - p1.x) * delta_Y / delta_X;
				p1.x = xMax;
			}

			if( K1 & CLIPLOWER ) {
				p1.x += (yMin-p1.y) * delta_X / delta_Y;
				p1.y = yMin;
			}
			else if( K1 & CLIPUPPER ) {
				p1.x += (yMax - p1.y) * delta_X / delta_Y;
				p1.y = yMax;
			}

			K1 = 0;

			if(p1.y < yMin) K1 =CLIPLOWER;
			if(p1.y > yMax) K1 =CLIPUPPER;
			if(p1.x < xMin) K1|=CLIPLEFT;
			if(p1.x > xMax) K1|=CLIPRIGHT;
		}
		
		if ( K2 ) {
			if( K2 & CLIPLEFT ) {
				p2.y += (xMin - p2.x) * delta_Y / delta_X;
				p2.x = xMin;
			} else if( K2 & CLIPRIGHT ) {
				 p2.y += (xMax - p2.x) * delta_Y / delta_X;
				 p2.x = xMax; 
			}
			
			if( K2 & CLIPLOWER ) {
				p2.x += (yMin - p2.y) * delta_X / delta_Y;
				p2.y = yMin;
			} else if( K2 & CLIPUPPER ) {
				p2.x += (yMax - p2.y) * delta_X / delta_Y;
				p2.y = yMax;
			}
			K2 = 0;

			if( p2.y < yMin ) K2 =CLIPLOWER;
			if( p2.y > yMax ) K2 =CLIPUPPER;
			if( p2.x < xMin ) K2|=CLIPLEFT;
			if( p2.x > xMax ) K2|=CLIPRIGHT;
		}
		break_while++;
	}
	}
}

void bhamLine(Point p1, Point p2, Color c)
{	
	int delta_X = p2.x - p1.x;
	int delta_Y = p2.y - p1.y;
	printf("%d, %d, %d, %d\n", p1.x, p1.y, p2.x, p2.x);
	//clipLine(p1, p2);
	printf("%d, %d, %d, %d\n", p1.x, p1.y, p2.x, p2.x);
	glBegin(GL_POINTS);
		glColor3f(c.r,c.g,c.b);
		glVertex2i(p1.x, p1.y);
		
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

CVec4f projectZ(CVec4f pView) {
	float x = pView.get(0);
	float y = pView.get(1);
	float z = pView.get(2);
	if ((fFocus - z) == 0) {
		printf("null :D");
	} 
	float k = fFocus / (fFocus - z);

	float xn = x * k;
	float yn = y * k;

	CVec4f pNew;
	float pNewArr[4] = {xn, yn, z, 1};
	pNew.setData(pNewArr);
	return pNew;

}

void drawProjektedZ(CVec4f points[8], Color c) {
	Point Points[8];
	for (int i = 0; i < 8; i++) {
		Points[i] = Point(points[i]);
	}
	if (points[0](2) < 0 || points[1](2) < 0) {
		bhamLine(Points[0], Points[1], c);
	}
	if (points[0](2) < 0 || points[3](2) < 0) {
		bhamLine(Points[0], Points[3], c);
	}
	if (points[0](2) < 0 || points[4](2) < 0) {
		bhamLine(Points[0], Points[4], c);
	}
	if (points[1](2) < 0 || points[2](2) < 0) {
		bhamLine(Points[1], Points[2], c);
	}
	if (points[1](2) < 0 || points[5](2) < 0) {
		bhamLine(Points[1], Points[5], c);
	}
	if (points[2](2) < 0 || points[3](2) < 0) {
		bhamLine(Points[2], Points[3], c);
	}
	if (points[2](2) < 0 || points[6](2) < 0) {
		bhamLine(Points[2], Points[6], c);
	}
	if (points[3](2) < 0 || points[7](2) < 0) {
		bhamLine(Points[3], Points[7], c);
	}
	if (points[4](2) < 0 || points[5](2) < 0) {
		bhamLine(Points[4], Points[5], c);
	}
	if (points[4](2) < 0 || points[7](2) < 0) {
		bhamLine(Points[4], Points[7], c);
	}
	if (points[5](2) < 0 || points[6](2) < 0) {
		bhamLine(Points[5], Points[6], c);
	}
	if (points[6](2) < 0 || points[7](2) < 0) {
		bhamLine(Points[6], Points[7], c);
	}
	/*bhamLine(Points[0], Points[1], c);
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
	bhamLine(Points[6], Points[7], c);*/
}

CVec4f projectZallg(CVec4f pWorld) {
	CVec4f pView = base_change_mat * trans(-viewOrigin) * pWorld;
	return projectZ(pView);
}

CMat4f baseChange() {
	float rot[4][4] = {{viewLeft(0), viewUp(0), -viewDir(0), 0},
					   {viewLeft(1), viewUp(1), -viewDir(1), 0},
					   {viewLeft(2), viewUp(2), -viewDir(2), 0},
					   {0,0,0,1}};
	return transpose(CMat4f(rot));
}

//     5------6
//    /|     /|
//   1------2 |
//   | 4----|-7
//   |/     |/
//   0------3
void drawQuader(Cuboid cuboid, float fFocus, Color c) {
	CVec4f points[8];
	for (int i = 0; i < 8; i++) {
		points[i] = projectZallg(cuboid.corners[i]);
	}
	drawProjektedZ(points, c);
}

CMat4f rotate_axis(CVec4f rot_axis, float angle) {
	float x = rot_axis(0);
	float y = rot_axis(1);
	float z = rot_axis(2);
	float c = cos(angle);
	float s = sin(angle);
	float a00 = c + pow(x,2) * (1 - c);
	float a01 = x * y * (1 - c) - z * s;
	float a02 = x * z * (1 - c) + y * s;
	float a10 = y * x * (1 - c) + z * s;
	float a11 = c + pow(y, 2) * (1 - c);
	float a12 = y * z * (1 - c) - x * s;
	float a20 = z * x * (1 - c) - y * s;
	float a21 = z * y * (1 - c) + x * s;
	float a22 = c + pow(z, 2)  * (1 - c);
	float arr[4][4] = {{a00, a01, a02, 0},
					   {a10, a11, a12, 0},
					   {a20, a21, a22, 0},
					   {0, 0, 0, 1}};
	return CMat4f(arr);
}

// function to initialize our own variables
void init () {
	// init timer interval
	g_iTimerMSecs = 50;

	fFocus = 2000;
	angle_deg = 5;
	angle_rad = angle_deg * 2 * pi / 360;
	stride = 10;
	float o_arr[4] = {0.0, 0.0, 0.0, 1.0};
	float d_arr[4] = {0.0, 0.0, -1.0, 1.0};
	float u_arr[4] = {0.0, 1.0, 0.0, 1.0};
	float l_arr[4] = {1.0, 0.0, 0.0, 1.0};
	viewOrigin.setData(o_arr);
	viewDir.setData(d_arr);
	viewUp.setData(u_arr);
	viewLeft.setData(l_arr);
	base_change_mat = baseChange();

	float c1_corner[4] = {0, 0, 0, 1};
	cuboid1 = Cuboid(CVec4f(c1_corner), 200);
	float c2_corner[4] = {-100, -100, 100, 1};
	cuboid2 = Cuboid(CVec4f(c2_corner), 100);

	
	
}

// function to initialize the view to ortho-projection
void initGL () {
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

// display callback function
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT);
	drawQuader(cuboid1, fFocus, Color(1,0,0));
	drawQuader(cuboid2, fFocus, Color(1,1,1));
	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
		case 'Q':
			exit (0); // quit program
			break;
		case 'F':
			fFocus += 10;
			break;
		case 'f': {
			fFocus -= 10;
			if (fFocus <= 100) {
				fFocus = 100;
			}
			break;
			}
		case 'U':
			viewOrigin.increment(0, stride);
			break;
		case 'u':
			viewOrigin.increment(0, -stride);
			break;
		case 'V':
			viewOrigin.increment(1, stride);
			break;
		case 'v':
			viewOrigin.increment(1, -stride);
			break;
		case 'W':
			viewOrigin.increment(2, stride);
			break;
		case 'w':
			viewOrigin.increment(2, -stride);
			break;
		case 'A':
			viewUp = rotate_axis(-viewDir, angle_rad) * viewUp;
			break;
		case 'a':
			viewUp = rotate_axis(-viewDir, -angle_rad) * viewUp;
			break;
		case 'B':
			viewDir = rotate_axis(viewUp, angle_rad) * viewDir;
			break;
		case 'b':
			viewDir = rotate_axis(viewUp, -angle_rad) * viewDir;
			break;
		case 'C':
			viewUp = rotate_axis(viewLeft, angle_rad) * viewUp;
			viewDir = rotate_axis(viewLeft, angle_rad) * viewDir;
			break;
		case 'c':
			viewUp = rotate_axis(viewLeft, -angle_rad) * viewUp;
			viewDir = rotate_axis(viewLeft, -angle_rad) * viewDir;
			break;
		case 'X':
			viewOrigin = rotx(angle_rad) * viewOrigin;
			break;
		case 'x':
			viewOrigin = rotx(-angle_rad) * viewOrigin;
			break;
		case 'Y':
			viewOrigin = roty(angle_rad) * viewOrigin;
			break;
		case 'y':
			viewOrigin = roty(-angle_rad) * viewOrigin;
			break;
		case 'Z':
			viewOrigin = rotz(angle_rad) * viewOrigin;
			break;
		case 'z':
			viewOrigin = rotz(-angle_rad) * viewOrigin;
			break;
		case 'r': {
				fFocus = 1000;
				float o_arr[3] = {0.0, 0.0, 0.0};
				float d_arr[3] = {0.0, 0.0, -1.0};
				float u_arr[3] = {0.0, 1.0, 0.0};
				viewOrigin.setData(o_arr);
				viewDir.setData(d_arr);
				viewUp.setData(u_arr);
				viewLeft = cross(viewUp, -viewDir);
			break;
		}
		default:
			// do nothing ...
			break;
	};
	viewLeft = cross(viewUp, -viewDir);
	base_change_mat = baseChange();
	glutPostRedisplay ();
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
	glutCreateWindow ("Uebung 3");

	init ();	// init my variables first
	initGL ();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutKeyboardFunc (keyboard);
	glutDisplayFunc (display);
	glutReshapeFunc(resize);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}
