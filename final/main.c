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
int distance  =   25;  // Light distance

int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int local     =   0;  // Local Viewer Model
int zh        =  90;  // Light azimuth
float ylight  =   15;  // Elevation of light
const char* text[] = {"Ortho","Top-Down Perspective","First Person"};

// Parameters for the green
int rows = 40;
int columns = 40;
float greenWidth = 10;
float greenDepth = 10;
float bumpiness = 0.2f; // Adjust this value for more/less bumpiness
float radiusX = 5.0f; 
float radiusZ = 3.0f;
quad** green;

//textures
unsigned int texture[5];

#define VELOCITY 0.5f


void drawPalmTreeLeaves(double x, double y, double z, int numLeaves, float leafRadius, int leafSegments) {
    // x,y,z is central point (top of trunk)
    for (int i = 0; i < numLeaves; i++) {
        float angleOffset = 2 * PI * i / numLeaves;
        // double Lx;
        // double Lz; 
        // Lx = leafRadius * Cos(i*angInc);
        // Lz = leafRadius * Sin(i*angInc);
        glPushMatrix();
        glRotatef(angleOffset *180/PI, 0, 1, 0);  // Rotate each leaf around y-axis
        drawPalmLeaf(x,y,z,leafRadius, leafSegments, -PI / 2, texture[1]);  // Draw leaf
        glPopMatrix();
    }
}
// Draw the tree with icosahedrons as leaves at a specified position
void drawTree(float x, float y, float z,float height) {
    glPushMatrix();
    glScalef(1,height,1);
    // Translate to the specified position
    glTranslatef(x, y, z);

    // Draw the trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown color
    drawTrunk(texture[0]);
    
    // Draw leaves as icosahedrons
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    
    // Translate to the top of the trunk
    glTranslatef(0,2,0);
    //glRotated();
    // Draw leaves as palm leaves
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    drawPalmTreeLeaves(0,0,0,   12,0.8,20);  // 12 leaves, radius 0.8, 20 segments per leaf
    drawPalmTreeLeaves(0,0.2f,0,    10, 0.5,20);  // 10 leaves, radius 0.5, 20 segments per leaf

    glPopMatrix();
    

}
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

int generateRandomInt(int min, int max){
   return rand() % (max - min + 1) + min;
}

/*
Function to populate the arrays, shape, etc for the current hole.
Params:
None? I think it's all global vars
Returns:
None, uses global vars
*/
void initializeHole(){
   int par = 3; //random choice from 3,4,5
   int holeLength = 10; //used to place green (i.e. z position)
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
      holeLength = 50; //obvious that something broke
         break;
   }
   green = createGreen(0,1,holeLength, rows,columns, 0.1, radiusX, radiusZ); // places green at the end of the hole

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
    dirx = Cos(ph)*Sin(th);
    diry = Sin(ph);
    dirz = -Cos(ph)*Cos(th);
    gluLookAt(xpos,ypos,zpos , dirx+xpos,diry+ypos,dirz+zpos , 0,1,0);
    ErrCheck("First Person");
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
    // ortho view
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }
    else if(mode==1){
    // top-down perspective
        topDownPerspective();
    }
    else if(mode==2){
    //first person perspective
    //camera position
        firstPersonPerspective();
    }
       //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[4];
      if(lightMode == 0){
         Position[0] =distance*Cos(30);
         Position[1]=ylight;
         Position[2]=distance*Sin(60);
         Position[3]=1.0;
      } 
      else if(lightMode == 1){
         Position[0] =distance*Cos(60);
         Position[1]=ylight;
         Position[2]=distance*Sin(0);
         Position[3]=1.0;
      } 
      else if(lightMode == 2){
         Position[0] =distance*Cos(180);
         Position[1]=ylight;
         Position[2]=distance*Sin(30);
         Position[3]=1.0;
      } 
      else if(lightMode == 3){
         Position[0]=distance*Cos(zh/15+180);
         Position[1]=ylight;
         Position[2]=distance*Sin(zh/15+180);
         Position[3]=1.0;
      }
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2],3.0);
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

   
   createTeeBox(3,6,3,1.8f);
   drawGround(texture[1]);
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
   //  Print the text string
   Print("Angle=%d,%d   Mode: %s, Light mode: %d",th,ph,text[mode],lightMode);
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
   Project(mode?fov:0,asp,dim);
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
        ph = 0;
        xpos=0;
        zpos=0;
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
   }
    if(mode==2){
        axes=0;
        if(ch == 'w'){
            zpos += VELOCITY * dirz;
            xpos += VELOCITY * dirx;
        }
        else if(ch == 's'){
            zpos -= VELOCITY * dirz;
            xpos -= VELOCITY * dirx;
        }
        else if(ch == 'a'){
            xpos += VELOCITY * dirz;
            zpos -= VELOCITY * dirx;
        }
        else if(ch == 'd'){
            xpos -= VELOCITY * dirz;
            zpos += VELOCITY * dirx;
        }
    }
    Project(mode?fov:0,asp,dim);
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
   double asp = (height>0) ? (double)width/height : 1;
   //setupPerspectiveCamera();
   //glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();

   Project(mode?fov:0,asp,dim);
}
/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(180*t,1440);
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
   //Initialize heights for drawing ground cells
   initializeHeights();
   
   green = createGreen(20, 1, 20, rows, columns, bumpiness,radiusX,radiusZ);
   // initializeHole();
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
//    //Load textures
   texture[0] = LoadTexBMP("textures/trunk.bmp");
   texture[1] = LoadTexBMP("textures/grass.bmp");
   texture[2] = LoadTexBMP("textures/water.bmp");
//    texture[3] = LoadTexBMP("brick.bmp");
//    texture[4] - LoadTexBMP("leaves.bmp");

   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   freeGreen(green, rows);
   return 0;
}
