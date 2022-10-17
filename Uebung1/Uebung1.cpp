///////////////////////////////////////////////////////////////////////
//																				         //
// Codeger�st f�r Vorlesung Computergraphik WS 2022/23 �bung 1       //
//										                                       //
///////////////////////////////////////////////////////////////////////

// Include-File f�r die Text Ein-/Ausgabe
#include <iostream>
using namespace std;

// Include-File f�r die GLUT-Library
#include "glut.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier wird einiges initialisiert. Sie d�rfen sich gerne den Code anschauen und
// versuchen ihn zu verstehen. Das sollte Ihnen nicht allzu schwer fallen. Es werden
// jedoch einige Techniken benutzt (Texturen, ...) die wir in der Vorlesung noch
// nicht besprochen haben, die wir aber aus techn. Gr�nden hier ben�tigen.
//
// Weiter unten finden Sie einen Vermerk, ab dem Sie den Code jedoch vollst�ndig
// verstehen sollten!!!

// Aufl�sungen der gesamten Textur
// !!!ACHTUNG!!! nicht alle Texturaufl�sungen funktionieren!
// Stichwort ungef�hr: POT2 Problematik
#define TEX_RES_X 60
#define TEX_RES_Y 60
// Anzahl der Pixel der Textur
#define TEX_RES TEX_RES_X *TEX_RES_Y
// Achsenl�nge der Textur (Achsen sind asymmetrisch von -TexRes#/2 bis TesRes#/2-1)
#define TEX_HALF_X TEX_RES_X / 2
#define TEX_HALF_Y TEX_RES_Y / 2
// Konvertiert Indices von (x,y) Koordinaten in ein lineares Array
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X * ((y)))

// globaler Speicher f�r Texturdaten
char g_Buffer[3 * TEX_RES];
// Textur ID Name
GLuint g_TexID = 0;

// Aufl�sung des Hauptfensters (kann sich durch User �ndern)
int g_WinWidth = 800;
int g_WinHeight = 800;

// Funktion organisiert die Textur.
// K�mmern Sie sich nicht weiter um diese Funktion, da
// sie momentan nur ein notwendiges �bel darstellt!
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

// Callback Funktion um die Fenstergr��en anzupassen.
// Auch diese Funktion ist ein notwendiges �bel! K�mmern
// Sie sich im Moment nicht weiter darum.
void reshape(int w, int h)
{

  g_WinWidth = w;
  g_WinHeight = h;
  glViewport(0, 0, w, h); // Establish viewing area to cover entire window.

  glMatrixMode(GL_PROJECTION);                 // Start modifying the projection matrix.
  glLoadIdentity();                            // Reset project matrix.
  glOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 1); // Map abstract coords directly to window coords.

  glutPostRedisplay();
}

//
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier f�ngt der f�r Sie wirklich relevante Teil des Programms an.
//

// Eine �beraus primitive Punktklasse
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

// Eine �beraus primitive Farbklasse
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

// Funktion l�scht den Bildschirm mit der angegebenen Farbe
// Usage z.B.: clearImage (Color (1,1,1))
// l�scht den Bildschirm in Wei�.
// Ohne Farbangabe ist der Standard Wei�
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

//
// �BUNG 1 AUFGABE 1:
//
// Diese Funktion soll eine Gerade zwischen den Punkten
// p1 und p2 in der Farbe c malen. Benutzen Sie die Funktion
// setPoint um die individuellen Punkte zu zeichnen.
void bhamLine(Point p1, Point p2, Color c)
{

  // erster Punkt
  setPoint(p1, c);

  // ...
  int x, y, dx, dy, d, dNE, dE;

  x = p1.x;
  y = p1.y;

  dx = p2.x - p1.x;
  dy = p2.y - p1.y;

  d = 2 * (dy - dx);

  dNE = 2 * (dy - dx);

  dE = 2 * dy;

  setPoint(Point(x, y), c);
  printf("Set point at (%d,%d)\n", x, y);

  while (x < p2.x)
  {
    printf("%d\n", d);
    if (d >= 0)
    {
      d += dNE;
      x++;
      y++;
      printf("if");
    }
    else
    {
      d += dE;
      x++;
      printf("else");
    }
    setPoint(Point(x, y), c);
    printf("Set point at (%d,%d)\n", x, y);
  }

  // letzter Punkt
  setPoint(p2, c);
}

//
// �BUNG 1 AUFGABE 2:
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

// Die Callback Funktion die f�r das eigentliche Malen
// zust�ndig ist. Im Wesentlichen brauchen Sie sich nur
// um den Bereich zwischen den Kommentaren zu k�mmern,
// alles andere ist wiederum ein notwendiges �bel!
void display(void)
{
  Color background(0.5, 0.5, 0.5); // grauer Hintergrund
  clearImage(background);          // alte Anzeige l�schen

  //////////////////////////////////////////////////////////////////
  //
  // Hier sollen Ihre Bresenham-Funktionen
  // eine Gerade und einen Kreis zeichnen.
  //
  // Im Prinzip brauchen Sie hier keine �nderungen vorzunehmen,
  // es sei denn Sie wollen "spielen" :-D
  //

  Point p1(-10, 20);      // ersten Punkt f�r Gerade definieren
  Point p2(20, -15);      // ebenso den zweiten Punkt
  Color cRed(1, 0, 0);    // Es soll eine rote Gerade sein ...
  bhamLine(p1, p2, cRed); // Gerade zeichnen ...

  Point p(-3, -5);         // Mittelpunkt f�r Kreis definieren
  int r = 17;              // Radius festlegen
  Color cBlue(0, 0, 1);    // Es soll ein blauer Kreis sein ...
  bhamCircle(p, r, cBlue); // Kreis zeichnen ...

  //
  // Ab hier sollten Sie nichts mehr �ndern!
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

  glutReshapeFunc(reshape); // zust�ndig f�r Gr��en�nderungen des Fensters
  glutDisplayFunc(display); // zust�ndig f�r das wiederholte Neuzeichnen des Bildschirms

  glutMainLoop();

  glDeleteTextures(1, &g_TexID); // l�scht die oben angelegte Textur

  return 0;
}
