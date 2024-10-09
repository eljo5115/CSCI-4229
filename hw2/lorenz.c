/*
 *  Coordinates
 *
 *  Display 2, 3 and 4 dimensional coordinates in 3D.
 *
 *  Key bindings:
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 *  +/-    Increase/decrease r
 *  q/e    Increase/decrease s
 *  z/c    increase/decrease size (w)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

bool _DEBUG = false;

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;
//  Globals
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
int mode=4;     // Dimension (1-4)
double z=0;     // Z variable
int w=25;     // W variable
double dim=50;   // Dimension of orthogonal box
const char* text[] = {"","2D","3D constant Z","3D","4D"};  // Dimension display text

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void lorenz(){
   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   float red = 0.0;
   float blue = 0.0;
   float green = 0.0;
   /*  Time step  */
   double dt = 0.001;
   if(_DEBUG){
      printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);
   }
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      if(_DEBUG){
         printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
      }
      red = i/50000.0;
      blue = i/50000.0;
      green = i/50000.0;
      if(_DEBUG){
         printf("color: %f, %f, %f\n",red,blue,green);  
      }
      if(i<17000){
         glColor3d(red,0,0);
      }else if(i>17000 && i<34000){
         glColor3d(0,blue,0);
      }else if(i>34000){
         glColor3d(0,0,green);
      }
      //glColor3d(x,y,z);
      glVertex4d(x,y,z,w);
   }
}

/*
 *  Display the scene
 */
void display()
{
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   //  Draw 10 pixel yellow points
   glPointSize(1);
   glBegin(GL_LINES);
   lorenz();
   glEnd();
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(1,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,1,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,1);
   glEnd();
   //  Label axes
   glRasterPos3d(1,0,0);
   Print("X");
   glRasterPos3d(0,1,0);
   Print("Y");
   glRasterPos3d(0,0,1);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   Print("View Angle=%d,%d, r=%f,s=%f,w=%d",th,ph,r,s,w);
   //  Flush and swap
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Increase z or w by 0.1
   else if (ch == '+')
   {
      r++;
   }
   //  Decrease z or w by 0.1
   else if (ch == '-')
   {
      r--;
   }
   else if (ch == 101)
   {
      s++;
   }
   else if (ch == 113){
      s--;
   }
   else if (ch == 122)
   {
      w--;
   }
   else if (ch == 99)
   {
      w++;
   }
   w %= 30;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   switch (key)
   {
   case GLUT_KEY_RIGHT:
   //  Right arrow key - increase azimuth by 5 degrees
      th += 5;
      break;
   case GLUT_KEY_LEFT:
   // //  Left arrow key - decrease azimuth by 5 degrees
      th -= 5;
      break;
   case GLUT_KEY_UP:
   // //  Up arrow key - increase elevation by 5 degrees
      ph+=5;
      break;
   case GLUT_KEY_DOWN:
   // //  Down arrow key - decrease elevation by 5 degrees
      ph-=5;
      break;
   default:
      printf("%d",key);
      break;
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Assignment 2: Eli Jordan");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
