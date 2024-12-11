/*
 *  3D Objects
 *
 *  Demonstrates how to draw objects in 3D.
 *
 *  Key bindings:
 *  m/M        Cycle through different views
 *  w,a,s,d    Move around the scene in first person
 *  a          Toggle axes
 *  arrows     Change view angle
 *  0          Reset view angle
 *  f/F to change light mode/position
 *  ESC        Exit
 */

#include "CSCIx229.h"
#include "drawObjects.h"

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



int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int axes=0;        //  Display axes
int mode=0;        //  What to display
int lightMode = 0;
double dim=30.0;    //orthogonal bounding box
int fov = 90;
double asp = 1;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool keys[256];

float angle = 0.0; // look angle in degrees
float pitch = 0.0; // look angle up/down in degrees

float xpos = 0.0f;
float ypos = 2.0f;
float zpos = 0.0f;
double dirx = 0.0;
double diry = 0.0;
double dirz = 0.0;
//lighting 
// Light values
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   50;  // Light distance

int ambient   =  0;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   20;  // Specular intensity (%)
int local     =   0;  // Local Viewer Model
int zh        =  90;  // Light azimuth
float ylight  =   40;  // Elevation of light
const char* text[] = {"Tee Box","Green View","First Person"};

// Parameters for the green
int rows = 40;
int columns = 40;
float greenWidth = 10;
float greenDepth = 10;
float radiusX = 5.0f; 
float radiusZ = 3.0f;
int holeLength;
int fairwayWidth;
int roughWidth;
int par = 0;
quad** green;
quad** fairway;
Point3D greenLoc;
//textures
unsigned int texture[5];

extern unsigned int textures[5];

#define VELOCITY 0.5f



// Function to draw a forest of trees
// draws numberOfTrees over a square area of areaSize*2
void drawForest(int numberOfTrees, float areaSize) {
    srand(FOREST_SEED); // Seed for random position generation

    for (int i = 0; i < numberOfTrees; ++i) {
        // Random positions within the defined area
        float x = ((float)rand() / RAND_MAX) * areaSize - (areaSize / 2);
        float z = ((float)rand() / RAND_MAX) * areaSize - (areaSize / 2);
        float y = 0.0f; // Assuming trees are planted on the ground plane (y=0)
        
        drawTree(x, y, z,(float)rand()/RAND_MAX+1.0f);
    }
}



/*
Function to populate the arrays, shape, etc for the current hole.
Params:
None? I think it's all global vars
Returns:
None, uses global vars
*/
void initializeHole(){
   srand(time(NULL));
   par = generateRandomInt(3,5); //random choice from 3,4,5
   holeLength = 10; //used to place green (i.e. z position)
   fairwayWidth = generateRandomInt(30,50);
   roughWidth = generateRandomInt(30,50);
   int numPoints = 8;
   switch(par){
      case 3:
      //random int between 160-220
         holeLength = generateRandomInt(160,220);
         break;
      case 4:
      //random int between 290-450
         holeLength = generateRandomInt(290,450);
         break;
      case 5:
      //random int between 500-650
         holeLength = generateRandomInt(490,650);
         break;
      default:
         holeLength = 20; //obvious that something broke
         break;
   }
   greenLoc.x = generateRandomInt(-10,10);
   greenLoc.y = 0;
   greenLoc.z = holeLength;
   green = createGreen(greenLoc.x,greenLoc.y,holeLength, rows,columns, radiusX, radiusZ); // places green at the end of the hole
   // Point3D* points = generateRandomControlPoints(numPoints,fairwayWidth,holeLength);
   fairway = createFairway(greenLoc,fairwayWidth,holeLength, roughWidth);
}

void topDownPerspective(){
        if(ph < 30){
            ph = 30;
        }else if(ph > 80){
            ph= 80;
        }
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey+20,Ez , 0,0,0 , 0,1,0);
}

void firstPersonPerspective(){
   ph = -10;
   th = -180;
    dirx = Cos(ph)*Sin(th);
    diry = Sin(ph);
    dirz = -Cos(ph)*Cos(th);
    ypos = 10;
    gluLookAt(xpos,ypos,zpos , dirx+xpos,diry+ypos,dirz+zpos , 0,1,0);
    ErrCheck("First Person");
}
void teeBoxView(){
   ph=-180;
   th = 0;
   dirx = Cos(ph)*Sin(th);
    diry = Sin(ph);
    dirz = -Cos(ph)*Cos(th);
   gluLookAt(0,5,0, dirx, diry+5, dirz, 0,1,0);
   ErrCheck("Tee Box Camera");
}

void greenView(){
   if(ph < 30){
      ph = 30;
   }else if(ph > 80){
      ph= 80;
   }
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   gluLookAt(greenLoc.x + Ex,Ey,greenLoc.z + Ez , greenLoc.x,greenLoc.y,greenLoc.z , 0,1,0);
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   // Enable face culling
   // glEnable(GL_CULL_FACE);

   //  Undo previous transformations
   glLoadIdentity();
    if(mode == 0){
    // tee box view
      teeBoxView();
    }
    else if(mode==1){
    // top-down perspective of green
        greenView();
    }
    else if(mode==2){
    //Flyover view of hole
        firstPersonPerspective();
    }
       //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
         float Ambient[] = {0.01*ambient, 0.01*ambient,0.01*ambient, 1.0};
         float Diffuse[] = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
         float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[4];

      if(lightMode == 0){
         Position[0]=distance*Cos(30);
         Position[1]=distance*Sin(30);
         Position[2]=holeLength+10;
         Position[3]=1.0;
         //morning color
         // float Ambient[] = {0.01*ambient, 0.01*ambient,0.01*ambient, 1.0};
         // float Diffuse[] = {0.02*diffuse ,0.03*diffuse ,0.05*diffuse ,1.0};
         // float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
      } 
      else if(lightMode == 1){
         Position[0]=distance*Cos(90);
         Position[1]=distance*Sin(90);
         Position[2]=holeLength+10;
         Position[3]=1.0;
         //noon color

      } 
      else if(lightMode == 2){
         Position[0] =distance*Cos(145);
         Position[1]=distance*Sin(145);
         Position[2]=holeLength+5;
         Position[3]=1.0;
         //late afternoon color
         // float Ambient[] = {0.01*ambient, 0.01*ambient,0.01*ambient, 1.0};
         // float Diffuse[] = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
         // float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
      } 
      else if(lightMode == 3){
         Position[0]=distance*Cos(180);
         Position[1]=distance*Sin(180);
         Position[2]=holeLength;
         Position[3]=1.0;
         //sunset color
         // float Ambient[] = {0.01*ambient, 0.01*ambient,0.01*ambient, 1.0};
         // float Diffuse[] = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
         // float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
      }
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,0.5,1);
      //ball(Position[0],Position[1],Position[2],3.0);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);
    //  Decide what to draw

    // Render the green
   drawGreen(green, rows, columns);
   //render the fairway
   drawFairway(fairway,holeLength,fairwayWidth+2*roughWidth);
   
   createTeeBox(3,6,3,1.8f);
   // drawGround(texture[1]);
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      const double len=25;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   glColor3ub(255,255,255);
   //  Print the text string
   Print("Par=%d - Yardage: %d Mode: %s, Light mode: %d",par,holeLength,text[mode],lightMode);
   //  Render the scene
   ErrCheck("display");

   glFlush();
   glutSwapBuffers();
}
/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT){
    //   if(mode == 2){
    //     angle += 5;
    //   }else
      th += 5;
   }
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT){
    // if(mode==2){
    //     angle-=5;
    // }else
      th -= 5;
   }
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP){
    // if(mode==2){
    //     pitch+=5;
    // }else
      ph += 5;
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN){
    // if(mode==2){
    //     angle-=5;
    // }else
      ph -= 5;
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   ErrCheck("Special");
   Project(mode?fov:45,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
void changeMode(int inc){
    if(inc){
        mode = (mode+1)%3;
    }else
        mode = (mode+2)%3;
    if(mode == 2){
        th=0;
        ph =-180;
        xpos=0;
        zpos=10;
    }
}
void changeLightMode(int inc){
   if(inc){
      lightMode = (lightMode+1)%4;
   }else{
      lightMode = (lightMode+2)%4;
   }
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
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == '-')
      dim-=1;
   else if (ch == '+')
      dim+=1 ;
    else if (ch == 'm')
        changeMode(1);
    else if (ch == 'M')
        changeMode(0);
   else if (ch == 'F'){
      //dec lightmode
      changeLightMode(0);
   }
   else if (ch == 'f'){
      //inc lightmode
      changeLightMode(1);
   }else if (ch == 'n'){
      initializeHole();
   }
   if(mode == 1){
      if(ch == 'w'){
         fov -= 3;
      }else if(ch=='s'){
         fov +=3;
      }
   }


    Project(mode?fov:60,asp,dim);
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
   //  Orthogonal projection
   // double asp = (height>0) ? (double)width/height : 1;
   //setupPerspectiveCamera();
   //glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();

   Project(mode?fov:45,asp,dim);
}
/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(180*t,1440);
   if(mode == 2 && zpos <= holeLength+30){
      zpos += holeLength/100;
   }else{
      zpos = 0;
   }
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   srand(FOREST_SEED);
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Eli Jordan Procedural Golf Course");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //set clear color to blue
   glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   // // for testing
   // green = createGreen(20, 1, 20, rows, columns,radiusX,radiusZ);
   // Point3D fairwayEnd;
   // fairwayEnd.x = 30;
   // fairwayEnd.y = 5;
   // fairwayEnd.z = 10;
   // fairway = createFairway(fairwayEnd, 8, 20, 10, 10);
   initializeHole();
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
//    //Load textures
   // textureStruct.tree = LoadTexBMP("textures/trunk.bmp");
   textures[0] = LoadTexBMP("textures/fairway.bmp");
   textures[1]= LoadTexBMP("textures/rough.bmp");
   textures[3] = LoadTexBMP("textures/sand.bmp");

   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   freeQuadArray(green, rows);
   freeQuadArray(fairway,holeLength);
   return 0;
}
