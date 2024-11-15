/*
 *  Anti-aliased lines
 *
 *  Draw jagged and anti-aliased lines
 *
 *  Key bindings:
 *  r/R  Change angle of lines
 *  ESC  Exit
 */
#include "CSCIx229.h"

// Draw 2 diagonal lines to form an X
static int th = 45;
void display(void)
{
   //  Clear screen
   glClear(GL_COLOR_BUFFER_BIT);

   //  Draw background quad
   const float dim=0.8;
   glBegin(GL_QUADS);
   glColor3f(1,1,0);
   glVertex2f(-0.8*dim,-0.4*dim);
   glVertex2f(-0.8*dim,+0.4*dim);
   glVertex2f(+0.8*dim,+0.4*dim);
   glVertex2f(+0.8*dim,-0.4*dim);
   glEnd();

   //  Two X's
   //  Left  aliased
   //  Right anti-aliased
   for (int i=-1;i<=1;i+=2)
   {
      //  Aliased line setup
      if (i<0)
      {
         glDisable (GL_LINE_SMOOTH);
         glDisable (GL_BLEND);
         glLineWidth (1.0);
      }
      //  Anti-aliased line setup
      else
      {
         glEnable(GL_LINE_SMOOTH);
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
         glLineWidth(1.5);
      }
      //  Draw X lines
      glBegin(GL_LINES);
      glColor3f(1,0,0);
      glVertex2f(-dim*Cos(th)+0.05*i,-dim*Sin(th));
      glVertex2f(+dim*Cos(th)+0.05*i,+dim*Sin(th));
      glColor3f(0,0,1);
      glVertex2f(-dim*Cos(th)+0.05*i,+dim*Sin(th));
      glVertex2f(+dim*Cos(th)+0.05*i,-dim*Sin(th));
      glEnd();
   }

   ErrCheck("display");
   glFlush();
}

//  Reshape callback
void reshape(int width,int height)
{
   //  Viewport full screen
   double asp = (height>0) ? (double)width/height : 1;
   glViewport(0,0, RES*width,RES*height);
   //  Set 2D orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(-asp,+asp,-1,+1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

//  Keyboard callbacks
void keyboard(unsigned char ch, int x, int y)
{
   //  Escape
   if (ch==27)
      exit(0);
   //  Change X angle
   else if (ch=='r')
      th = (th-5)%360;
   else if (ch=='R')
      th = (th+5)%360;
   glutPostRedisplay();   
}

//  Main
int main(int argc, char** argv)
{
   //  Initialize
   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA);
   glutInitWindowSize(200,200);
   glutCreateWindow("Anti-aliasing");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(display);
   //  Mainloop
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
