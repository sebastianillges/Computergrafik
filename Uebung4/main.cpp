#include <math.h>
#include "vec.h"
#include "mat.h"
#include "glut.h"
#include "utility.h"
#include <iostream>
using namespace std;

// system relevant global variables
// window width and height (choose an appropriate size)
const int g_iWidth  = 900;
const int g_iHeight = 900;

// global variable to tune the timer interval
int g_iTimerMSecs;

// sphere data
CVec3f bodyCenter;
float bodyRadius;
Color bodyColor;
float bodyShininess;

// cube planes
CVec3f frontPlaneNormal;
float frontPlanePosition;
CVec3f topPlaneNormal;
float topPlanePosition;

// view system
CVec3f viewOrigin;
CVec3f viewDir;
CVec3f xAxis;
CVec3f yAxis;
CVec3f zAxis;

// lightsource
CVec3f lightPos;
CVec3f lightColor;
float lightIntensity;
float ambientIntensity;

float angle_deg;
float angle_rad;
float stride;

float xMin = (-g_iWidth / 2);
float xMax = (g_iWidth / 2) - 1;
float yMin = (-g_iHeight / 2);
float yMax = ((g_iHeight / 2)) - 1;

Point project(CVec3f p) {
	float k = viewOrigin.get(2) / (viewOrigin.get(2) - p.get(2));
	Point pNew = Point(p.get(0) * k, p.get(1) * k);
	return pNew;
}

CMat3f rotate_axis(CVec3f rot_axis, float angle)
{
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
	float arr[3][3] = {{a00, a01, a02},
					   {a10, a11, a12},
					   {a20, a21, a22}};
	return CMat3f(arr);
}

Color phongSphere(CVec3f HitPos, CVec3f EyePos)
{
    CVec3f N;
    CVec3f L;
    CVec3f V;
    float N_arr[3] = {HitPos.get(0) - bodyCenter.get(0),
                          HitPos.get(1) - bodyCenter.get(1),
                          HitPos.get(2) - bodyCenter.get(2)};
    float L_arr[3] = {lightPos.get(0) - HitPos.get(0),
                      lightPos.get(1) - HitPos.get(1),
                      lightPos.get(2) - HitPos.get(2)};
    float V_arr[3] = {EyePos.get(0) - HitPos.get(0),
                      EyePos.get(1) - HitPos.get(1),
                      EyePos.get(2) - HitPos.get(2)};
    N.setData(N_arr);
    N = N.normalize();
    L.setData(L_arr);
    L = L.normalize();
    V.setData(V_arr);
    V = V.normalize();

    CVec3f R = rotate_axis(N, M_PI) * L;
    
    float Ia = ambientIntensity;                                    // ambient
    float Id = skalarProd(N, L) * lightIntensity;                   // diffuse
    float Is = pow(skalarProd(R, V), bodyShininess) * lightIntensity;   // specular

    CVec3f kd;
    float kd_arr[3] = {bodyColor.r, bodyColor.g, bodyColor.b};
    kd.setData(kd_arr);
    CVec3f ks = lightColor;
    CVec3f ka = kd;

    CVec3f finalColor = ((kd * max(0.0f, Id)) + (ks * max(0.0f, Is)) * lightIntensity) + ka * Ia;
	//CVec3f finalColor = ka * Ia + kd * Id + ks * Is;
	//CVec3f finalColor = ((kd * max(0.0f, Id)) + (ks * max(0.0f, Is)) * lightIntensity);

    Color phongColor = Color(finalColor.get(0), finalColor.get(1), finalColor.get(2));
    return phongColor;
}

Color phongCube(CVec3f HitPos, CVec3f EyePos, CVec3f planeNormal)
{
//     		_______
//    	   /|  3  /|
//   	  /______/ |
//   	4 | |____|_| 2
//   	  | /  1 | /
//   	  |/_____|/
//			 5
	
    CVec3f L;
    CVec3f V;
    float L_arr[3] = {lightPos.get(0) - HitPos.get(0),
                      lightPos.get(1) - HitPos.get(1),
                      lightPos.get(2) - HitPos.get(2)};
    float V_arr[3] = {EyePos.get(0) - HitPos.get(0),
                      EyePos.get(1) - HitPos.get(1),
                      EyePos.get(2) - HitPos.get(2)};
    L.setData(L_arr);
    L = L.normalize();
    V.setData(V_arr);
    V = V.normalize();

    CVec3f R = rotate_axis(planeNormal, M_PI) * L;
    
    float Ia = ambientIntensity;                                    // ambient
    float Id = skalarProd(planeNormal, L) * lightIntensity;                   // diffuse
    float Is = pow(skalarProd(R,V), bodyShininess) * lightIntensity;// specular

    CVec3f kd;
    float kd_arr[3] = {bodyColor.r, bodyColor.g, bodyColor.b};
    kd.setData(kd_arr);
    CVec3f ks = lightColor;
    CVec3f ka = kd;

    CVec3f finalColor = ((kd * max(0.0f, Id)) + (ks * max(0.0f, Is)) * lightIntensity) + ka * Ia;

    Color phongColor = Color(finalColor.get(0), finalColor.get(1), finalColor.get(2));
    return phongColor;
}

CVec3f intersectSphere(CVec3f EyePos, CVec3f ViewDir)
{
    // t = (-(e - m) +/- sqrt((e - m)^2 - v^2 * r^2)) / v^2
    // Meine
    /*
    CVec3f EyeMinusCenter = EyePos - bodyCenter;
    CVec3f EyeMinusCenterSqr = EyeMinusCenter.square();
    CVec3f ViewSqrTimesRadiusSqr = ViewDir.square() * pow(bodyRadius, 2);
    CVec3f underRoot = EyeMinusCenterSqr - ViewSqrTimesRadiusSqr;
    CVec3f root = underRoot.squareRoot();

    CVec3f t1 = (-(EyeMinusCenter) + root) / ViewDir.square();
    CVec3f t2 = (-(EyeMinusCenter) - root) / ViewDir.square();

    return (t1.length() < t2.length()) ? (t1) : (t2);
    */

    // ChatGPT

    double a = ViewDir.get(0) * ViewDir.get(0)
             + ViewDir.get(1) * ViewDir.get(1)
             + ViewDir.get(2) * ViewDir.get(2);
    double b = 2 * (ViewDir.get(0) * (EyePos.get(0) - bodyCenter.get(0))
                  + ViewDir.get(1) * (EyePos.get(1) - bodyCenter.get(1))
                  + ViewDir.get(2) * (EyePos.get(2) - bodyCenter.get(2)));
    double c = (EyePos.get(0) - bodyCenter.get(0)) * (EyePos.get(0) - bodyCenter.get(0))
             + (EyePos.get(1) - bodyCenter.get(1)) * (EyePos.get(1) - bodyCenter.get(1))
             + (EyePos.get(2) - bodyCenter.get(2)) * (EyePos.get(2) - bodyCenter.get(2))
             - bodyRadius * bodyRadius;
    double d = b * b - 4 * a * c;

    CVec3f intersection;

    if (d < 0)
    {
        intersection.set(0, 0);
        intersection.set(1, 0);
        intersection.set(2, -1);
        return intersection;
        // no real solutions
    }
    else if (d == 0)
    {
        // one solution
        float t = -b / (2 * a);
        intersection.set(0, EyePos.get(0) + t * ViewDir.get(0));
        intersection.set(1, EyePos.get(1) + t * ViewDir.get(1));
        intersection.set(2, EyePos.get(2) + t * ViewDir.get(2));
        return intersection;
    }
    else
    {
        // two solutions
        float t1 = (-b + sqrt(d)) / (2 * a);
        float t2 = (-b - sqrt(d)) / (2 * a);

        (t1 < t2) || (t1 = t2);

        intersection.set(0, EyePos.get(0) + t1 * ViewDir.get(0));
        intersection.set(1, EyePos.get(1) + t1 * ViewDir.get(1));
        intersection.set(2, EyePos.get(2) + t1 * ViewDir.get(2));
        return intersection;
    }
}

void drawSphere(CVec3f EyePos, CVec3f ViewDir)
{
    for (int i = xMin; i < xMax; i++)
    {
        for (int j = yMin; j < yMax; j++)
        {
            ViewDir.set(0, i);
            ViewDir.set(1, j);
            CVec3f interPoint = intersectSphere(EyePos, ViewDir);
            if (interPoint.get(2) != -1)
            {
                Point p = project(interPoint);
                Color c = phongSphere(interPoint, EyePos);
                glBegin(GL_POINTS);
                    glColor3f(c.r, c.g, c.b);
                    //glColor3f(bodyColor.r, bodyColor.g, bodyColor.b);
                    glVertex2i(p.x, p.y);
                    glVertex2i(p.x+1, p.y);
                    glVertex2i(p.x-1, p.y);
                    glVertex2i(p.x, p.y+1);
                    glVertex2i(p.x, p.y-1);
                glEnd();
            }
        }
    }
}

void drawCube(CVec3f EyePos, CVec3f ViewDir)
{
	for (int i = bodyCenter.get(0) - bodyRadius; i < bodyCenter.get(0) + bodyRadius; i++)
    {
        for (int j =  bodyCenter.get(1) - bodyRadius; j <  bodyCenter.get(1) + bodyRadius; j++)
        {
            ViewDir.set(0, i);
            ViewDir.set(1, j);
            CVec3f interPoint;
			float interPoint_arr[3] = {(float) i, (float) j, bodyCenter.get(2) + bodyRadius};
			interPoint.setData(interPoint_arr);
			Point p = project(interPoint);
			Color c = phongCube(interPoint, EyePos, frontPlaneNormal);
			glBegin(GL_POINTS);
				glColor3f(c.r, c.g, c.b);
				//glColor3f(bodyColor.r, bodyColor.g, bodyColor.b);
				glVertex2i(p.x, p.y);
				glVertex2i(p.x+1, p.y);
				glVertex2i(p.x-1, p.y);
				glVertex2i(p.x, p.y+1);
				glVertex2i(p.x, p.y-1);
			glEnd();
        }
    }
	for (int i = bodyCenter.get(0) - bodyRadius; i < bodyCenter.get(0) + bodyRadius; i++)
    {
        for (int j = bodyCenter.get(2) - bodyRadius; j < bodyCenter.get(2) + bodyRadius; j++)
        {
            ViewDir.set(0, i);
            ViewDir.set(2, j);
            CVec3f interPoint;
			float interPoint_arr[3] = {(float) i, bodyCenter.get(1) + bodyRadius, (float) j};
			interPoint.setData(interPoint_arr);
			Point p = project(interPoint);
			Color c = phongCube(interPoint, EyePos, topPlaneNormal);
			glBegin(GL_POINTS);
				glColor3f(c.r, c.g, c.b);
				//glColor3f(bodyColor.r, bodyColor.g, bodyColor.b);
				glVertex2i(p.x, p.y);
				glVertex2i(p.x+1, p.y);
				glVertex2i(p.x-1, p.y);
				glVertex2i(p.x, p.y+1);
				glVertex2i(p.x, p.y-1);
			glEnd();
        }
    }
}

// function to initialize our own variables
void init () {
	// init timer interval
	g_iTimerMSecs = 50;

	angle_deg = 5;
	angle_rad = angle_deg * 2 * 3.14159265359 / 360;
	stride = 10;

	float o_arr[3] = {0.0, 0.0, 300};
	float d_arr[3] = {0.0, 0.0, -300};
    float x_arr[3] = {1.0, 0.0, 0.0};
    float y_arr[3] = {0.0, 1.0, 0.0};
    float z_arr[3] = {0.0, 0.0, 1.0};
	viewOrigin.setData(o_arr);
	viewDir.setData(d_arr);
    xAxis.setData(x_arr);
    yAxis.setData(y_arr);
    zAxis.setData(z_arr);

    float lPos_arr[3] = {0, 1000, 0};
    float lCol_arr[3] = {1, 1, 1};
    lightPos.setData(lPos_arr);
	lightPos = rotz(M_PI / 4) * lightPos;
    lightColor.setData(lCol_arr);
    lightIntensity = 1;
	ambientIntensity = 0.08;

    float ichHaseMeinLeben[3] = {0, -300, -200};
	bodyCenter.setData(ichHaseMeinLeben);
    bodyRadius = 200;
    bodyColor = Color(1, 1, 1);
    bodyShininess = 7;

	float frontPlaneNormal_arr[3] = {1, 0, 0};
	frontPlanePosition = bodyCenter.get(2) + bodyRadius;
	float topPlaneNormal_arr[3] = {0, 1, 0};
	topPlaneNormal.setData(topPlaneNormal_arr);
	topPlanePosition = bodyCenter.get(1) + bodyRadius;
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
void display1 (void) {
	glClear (GL_COLOR_BUFFER_BIT);

    drawSphere(viewOrigin, viewDir);

	// In double buffer mode the last
	// two lines shouId alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

void display2 (void) {
	glClear (GL_COLOR_BUFFER_BIT);

    drawCube(viewOrigin, viewDir);

	// In double buffer mode the last
	// two lines shouId alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
		case 'Q':
			exit (0); // quit program
			break;
		case 'i':
			printf("Shininess: %f, Lightintensity: %f, Color(%f, %f, %f)\n", bodyShininess, lightIntensity, bodyColor.r, bodyColor.g, bodyColor.b);
			break;
        case 'o':
            bodyRadius+=stride;
            break;
        case 'O':
            bodyRadius-=stride;
            break;
		case '+':
			(ambientIntensity < 1) && (ambientIntensity+=0.01);
			break;
		case '-':
			(ambientIntensity > 0) && (ambientIntensity-=0.01);
			break;
        case 'p':
			(bodyShininess >= 2) && (bodyShininess-=2);
			(bodyShininess >= 2) && (bodyShininess-=2);
            break;
        case 'P':
            bodyShininess+=2;
            break;
        case 'l':
            (lightIntensity > 0) && (lightIntensity-=0.05);
            break;
        case 'L':
            lightIntensity+=0.05;
            break;
        case 'r':
            (bodyColor.r > 0) && (bodyColor.r-=0.05);
            break;
        case 'R':
            (bodyColor.r < 1) && (bodyColor.r+=0.05);
            break;
        case 'g':
            (bodyColor.g > 0) && (bodyColor.g-=0.05);
            break;
        case 'G':
            (bodyColor.g < 1) && (bodyColor.g+=0.05);
            break;
        case 'b':
            (bodyColor.b > 0) && (bodyColor.b-=0.05);
            break;
        case 'B':
            (bodyColor.b < 1) && (bodyColor.b+=0.05);
            break;
        case 'X':
            lightPos = rotate_axis(xAxis, angle_rad) * lightPos;
			break;
		case 'x':
			lightPos = rotate_axis(xAxis, -angle_rad) * lightPos;
			break;
		case 'Y':
			lightPos = rotate_axis(yAxis, angle_rad) * lightPos;
			break;
		case 'y':
			lightPos = rotate_axis(yAxis, -angle_rad) * lightPos;
			break;
		case 'Z':
			lightPos = rotate_axis(zAxis, angle_rad) * lightPos;
			break;
		case 'z':
			lightPos = rotate_axis(zAxis, -angle_rad) * lightPos;
			break;
		case 'u':
			bodyCenter.set(1, bodyCenter.get(1) + 10);
			break;
		case 'h':
			bodyCenter.set(0, bodyCenter.get(0) - 10);
			break;
		case 'j':
			bodyCenter.set(1, bodyCenter.get(1) - 10);
			break;
		case 'k':
			bodyCenter.set(0, bodyCenter.get(0) + 10);
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
	glutPostRedisplay ();
}

void resize(int width, int height) {
    // we ignore the params and do:
    glutReshapeWindow(g_iWidth, g_iHeight);
}

int main (int argc, char **argv) 
{
	glutInit (&argc, argv);
	// we have to use double buffer to avoid flickering
	// TODO: lookup "double buffer", what is it for, how is it used ...
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutCreateWindow ("Uebung 4");

	init ();	// init my variables first
	initGL ();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutKeyboardFunc (keyboard);
	glutDisplayFunc (display1);
	glutReshapeFunc(resize);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}