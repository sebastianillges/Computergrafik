///////////////////////////////////////////////////////////////////////
//																				                           //
// Codegerüst für Vorlesung Computergraphik WS 2022/23 Übung 1       //
//										                                               //
///////////////////////////////////////////////////////////////////////

// Include-File für die Text Ein-/Ausgabe
#include <iostream>
using namespace std;

// Include-File für die GLUT-Library
#include "glut.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier wird einiges initialisiert. Sie dürfen sich gerne den Code anschauen und
// versuchen ihn zu verstehen. Das sollte Ihnen nicht allzu schwer fallen. Es werden
// jedoch einige Techniken benutzt (Texturen, ...) die wir in der Vorlesung noch
// nicht besprochen haben, die wir aber aus techn. Gründen hier benötigen.
//
// Weiter unten finden Sie einen Vermerk, ab dem Sie den Code jedoch vollständig
// verstehen sollten!!!

// Auflösungen der gesamten Textur
// !!!ACHTUNG!!! nicht alle Texturauflösungen funktionieren!
// Stichwort ungefähr: POT2 Problematik
#define TEX_RES_X 60
#define TEX_RES_Y 60
// Anzahl der Pixel der Textur
#define TEX_RES TEX_RES_X *TEX_RES_Y
// Achsenlänge der Textur (Achsen sind asymmetrisch von -TexRes#/2 bis TesRes#/2-1)
#define TEX_HALF_X TEX_RES_X / 2
#define TEX_HALF_Y TEX_RES_Y / 2
// Konvertiert Indices von (x,y) Koordinaten in ein lineares Array
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X * ((y)))

// globaler Speicher für Texturdaten
char g_Buffer[3 * TEX_RES];
// Textur ID Name
GLuint g_TexID = 0;

// Auflösung des Hauptfensters (kann sich durch User ändern)
int g_WinWidth = 800;
int g_WinHeight = 800;

// Funktion organisiert die Textur.
// Kümmern Sie sich nicht weiter um diese Funktion, da
// sie momentan nur ein notwendiges übel darstellt!
void manageTexture()
{
	glEnable(GL_TEXTURE_2D);
	if (g_TexID == 0)
		glGenTextures(1, &g_TexID);
	glBindTexture(GL_TEXTURE_2D, g_TexID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, g_Buffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

// Callback Funktion um die Fenstergrößen anzupassen.
// Auch diese Funktion ist ein notwendiges übel! Kümmern
// Sie sich im Moment nicht weiter darum.
void reshape(int w, int h)
{

	g_WinWidth = w;
	g_WinHeight = h;
	glViewport(0, 0, w, h); // Establish viewing area to cover entire window.

	glMatrixMode(GL_PROJECTION);				 // Start modifying the projection matrix.
	glLoadIdentity();							 // Reset project matrix.
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 1); // Map abstract coords directly to window coords.

	glutPostRedisplay();
}

//
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier fängt der für Sie wirklich relevante Teil des Programms an.
//

// Eine überaus primitive Punktklasse
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

// Eine überaus primitive Farbklasse
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

// Funktion löscht den Bildschirm mit der angegebenen Farbe
// Usage z.B.: clearImage (Color (1,1,1))
// löscht den Bildschirm in Weiß.
// Ohne Farbangabe ist der Standard Weiß
void clearImage(Color c = Color())
{
	for (int i = 0; i < TEX_RES; i++)
	{
		g_Buffer[3 * i] = 255.0 * c.r;
		g_Buffer[3 * i + 1] = 255.0 * c.g;
		g_Buffer[3 * i + 2] = 255.0 * c.b;
	}
}

// Funktion malt einen Punkt an die angegebenen Koordinaten
// Usage z.B.: setPoint (Point(10, 5), Color (1,0,0))
// malt einen Punkt an die Koordinate (10, 5)
// Ohne Farbangabe ist die Standard-Malfarbe Schwarz
//
// Nutzen Sie diese Funktion ...
void setPoint(Point p, Color c = Color(0, 0, 0))
{
	int x = p.x + TEX_HALF_X;
	int y = p.y + TEX_HALF_Y;
	if (x < 0 || y < 0 || x >= TEX_RES_X || y >= TEX_RES_Y)
	{
		cerr << "Illegal point co-ordinates (" << p.x << ", " << p.y << ")\n"
			 << flush;
		return;
	}

	g_Buffer[3 * TO_LINEAR(x, y)] = 255.0 * c.r;
	g_Buffer[3 * TO_LINEAR(x, y) + 1] = 255.0 * c.g;
	g_Buffer[3 * TO_LINEAR(x, y) + 2] = 255.0 * c.b;
}

Point flipPoint(Point p, int octant)
{
	switch (octant) 
	{
		case 2: return Point(p.y,p.x);
		case 3: return Point(-p.y,p.x);
		case 4:	return Point(-p.x,p.y);
		case 5:	return Point(-p.x,-p.y);
		case 6:	return Point(-p.y,-p.x);
		case 7:	return Point(-p.y,p.x);
		case 8:	return Point(p.x,-p.y);
		default: return p;
	}
}

Point backFlip(Point p, int octant)
{
	switch (octant) 
	{
		case 2: return Point(p.y,p.x);
		case 3: return Point(p.y,-p.x);
		case 4:	return Point(-p.x,p.y);
		case 5:	return Point(-p.x,-p.y);
		case 6:	return Point(-p.y,-p.x);
		case 7:	return Point(p.y,-p.x);
		case 8:	return Point(p.x,-p.y);
		default: return p;
	}
}

//
// ÜBUNG 1 AUFGABE 1:
//
// Diese Funktion soll eine Gerade zwischen den Punkten
// p1 und p2 in der Farbe c malen. Benutzen Sie die Funktion
// setPoint um die individuellen Punkte zu zeichnen.
void bhamLine(Point p1, Point p2, Color c)
{

	// erster Punkt
	setPoint(p1, c);

	// ...
	int x, y, dx, dy, d, dNE, dE, octant;
	octant = 1;
	x = p1.x;
	y = p1.y;
	
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	
	// d = 2 * dy - dx;

	if (abs(dx) > abs(dy)) // 0 < slope < 1 (ocatants 1,4,5,8)
	{
		if (dx < 0) // octants 4,5
		{
			if (dy < 0) {octant = 4;} // octant 4
			else {octant = 5;} // octant 5
		}
		else // octant 1,8
		{
			if (dy < 0) {octant = 8;} // octant 8
			else {octant = 1;} // octant 1
		}
	}
	else // octant 2,3,6,7
	{
		if (dx < 0) // octant 3,6
		{
			if (dy < 0) {octant = 6;} // octant 6
			else {octant = 3;} // octant 3
		}
		else // octant 2,7
		{
			if (dy < 0) {octant = 7;} // octant 7
			else {octant = 2;} // octant 2
		}
	}
	// flip p2
	Point p2flip = flipPoint(p2, octant);
	printf("%d,%d\n", p2flip.x, p2flip.y);
	// dX, dY, dNE, dE mit geflippten p2 berechnen
	dx = p2flip.x - p1.x;
	dy = p2flip.y - p1.y;
	d = 2 * dy - dx;
	// bresenham algo, vor jedem setPixel den Pixel zurückflippen
	
	dNE = 2 * (dy - dx);
	dE = 2 * dy;

	while (x < p2flip.x)
	{
		if (d >= 0)
		{
			d += dNE;
			x++;
			y++;
		}
		else
		{
			d += dE;
			x++;
		}
		Point p = backFlip(Point(x, y), octant);
		printf("%d,%d\n", p.x, p.y);
		setPoint(p, c);
	}

	setPoint(p2, c); // letzter Punkt
}

//
// ÜBUNG 1 AUFGABE 2:
//
// Diese Funktion soll einen Kreis mit Radius r um den Punkt p
// malen. Benutzen Sie die Funktion setPoint um die individuellen
// Punkte zu zeichnen. Vergessen Sie nicht auch den Mittelpunkt zu
// zeichnen!
void bhamCircle(Point p, int r, Color c)
{

	// Mittelpunkt
	setPoint(p, c);

	// ...
}

// Die Callback Funktion die für das eigentliche Malen
// zuständig ist. Im Wesentlichen brauchen Sie sich nur
// um den Bereich zwischen den Kommentaren zu kümmern,
// alles andere ist wiederum ein notwendiges übel!
void display(void)
{
	Color background(0.5, 0.5, 0.5); // grauer Hintergrund
	clearImage(background);			 // alte Anzeige löschen

	//////////////////////////////////////////////////////////////////
	//
	// Hier sollen Ihre Bresenham-Funktionen
	// eine Gerade und einen Kreis zeichnen.
	//
	// Im Prinzip brauchen Sie hier keine Änderungen vorzunehmen,
	// es sei denn Sie wollen "spielen" :-D
	//

	Point p1(-10, 20); // ersten Punkt für Gerade definieren
	// Point p2(-5, 29);
	Point p2(20, -15);		// ebenso den zweiten Punkt
	Color cRed(1, 0, 0);	// Es soll eine rote Gerade sein ...
	bhamLine(p1, p2, cRed); // Gerade zeichnen ...

	Point p(-3, -5);		 // Mittelpunkt für Kreis definieren
	int r = 17;				 // Radius festlegen
	Color cBlue(0, 0, 1);	 // Es soll ein blauer Kreis sein ...
	bhamCircle(p, r, cBlue); // Kreis zeichnen ...

	//
	// Ab hier sollten Sie nichts mehr ändern!
	//
	//////////////////////////////////////////////////////////////////

	manageTexture();

	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g_TexID);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex2f(-g_WinWidth / 2, -g_WinHeight / 2);
	glTexCoord2f(1, 0);
	glVertex2f(g_WinWidth / 2, -g_WinHeight / 2);
	glTexCoord2f(1, 1);
	glVertex2f(g_WinWidth / 2, g_WinHeight / 2);
	glTexCoord2f(0, 1);
	glVertex2f(-g_WinWidth / 2, g_WinHeight / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glFlush();
}

// Die Main-Funktion
int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitWindowSize(g_WinWidth, g_WinHeight);
	glutCreateWindow("Übung 1: Bresenham");

	glutReshapeFunc(reshape); // zuständig für Größenänderungen des Fensters
	glutDisplayFunc(display); // zustandig für das wiederholte Neuzeichnen des Bildschirms

	glutMainLoop();

	glDeleteTextures(1, &g_TexID); // löscht die oben angelegte Textur

	return 0;
}