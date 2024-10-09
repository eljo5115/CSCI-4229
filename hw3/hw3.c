/*
 *  3D Objects
 *
 *  Demonstrates how to draw objects in 3D.
 *
 *  Key bindings:
 *  m/M        Cycle through different sets of objects
 *  a          Toggle axes
 *  arrows     Change view angle
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
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
double zh=0;       //  Rotation of teapot
int axes=0;        //  Display axes
int mode=0;        //  What to display
double dim=30;    //orthogonal bounding box
const char* text[] = {"Cuboids","Spheres","FlatPlane Outline","FlatPlane Fill","SolidPlane","Icosahedron DrawElements","Icosahedron DrawArrays","Icosahedron VBO","Scene"};

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))
#define FOREST_SEED 136
#define GRID_SIZE 50  // Number of grid cells along one axis
#define HALF_GRID_SIZE GRID_SIZE/2
#define CELL_SIZE 1.0f  // Size of each cell in the grid
// River parameters
#define RIVER_WIDTH 2.5f
#define RIVER_DEPTH 1.0f
/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
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
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
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

void drawRectangle(float width, float height, float depth) {
    glBegin(GL_QUADS);
    
    // Front face
    glVertex3f(-width / 2, 0, depth / 2);
    glVertex3f(width / 2, 0, depth / 2);
    glVertex3f(width / 2, height, depth / 2);
    glVertex3f(-width / 2, height, depth / 2);
    
    // Back face
    glVertex3f(-width / 2, 0, -depth / 2);
    glVertex3f(width / 2, 0, -depth / 2);
    glVertex3f(width / 2, height, -depth / 2);
    glVertex3f(-width / 2, height, -depth / 2);
    
    // Left face
    glVertex3f(-width / 2, 0, -depth / 2);
    glVertex3f(-width / 2, 0, depth / 2);
    glVertex3f(-width / 2, height, depth / 2);
    glVertex3f(-width / 2, height, -depth / 2);
    
    // Right face
    glVertex3f(width / 2, 0, -depth / 2);
    glVertex3f(width / 2, 0, depth / 2);
    glVertex3f(width / 2, height, depth / 2);
    glVertex3f(width / 2, height, -depth / 2);
    
    // Top face
    glVertex3f(-width / 2, height, depth / 2);
    glVertex3f(width / 2, height, depth / 2);
    glVertex3f(width / 2, height, -depth / 2);
    glVertex3f(-width / 2, height, -depth / 2);
    
    glEnd();
}

// Function to draw a cone, serving as the foliage
void drawCone(float radius, float height, int slices) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height, 0);  // Apex of the cone
    for (int i = 0; i <= slices; ++i) {
        float angle = i * 2.0f * M_PI / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, 0, z);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);  // Center of the base
    for (int i = 0; i <= slices; ++i) {
        float angle = i * 2.0f * M_PI / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, 0, z);
    }
    glEnd();
}

// Function to draw the tree
void drawTree(float x, float y, float z,float height) {
    glPushMatrix();
    glScalef(1,height,1);
    // Draw the trunk
    glColor3f(0.55f, 0.27f, 0.07f);  // Brown color
    glPushMatrix();
    glTranslatef(x, y,z);
    drawRectangle(0.2f, 1.0f, 0.2f);
    glPopMatrix();

    // Draw the foliage
    glColor3f(0.0f, 0.5f, 0.0f);  // Green color
    glPushMatrix();
    glTranslatef(x, y+1.0f,z);
    drawCone(0.5f, 1.0f, 20);
    glPopMatrix();

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
        
        drawTree(x, y, z,(float)rand()/RAND_MAX);
    }
}


// Array to store heights for the grid
float heights[GRID_SIZE][GRID_SIZE];

// Function to initialize heights for the grid
void initializeHeights() {
    srand(FOREST_SEED);
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            float x = (float)i / (GRID_SIZE - 1);
            float y = (float)j / (GRID_SIZE - 1);
            float hill1 = 0.5f * expf(-((x - 0.3f) * (x - 0.3f) + (y - 0.3f) * (y - 0.3f)) / 0.02f);
            float hill2 = 0.3f * expf(-((x - 0.7f) * (x - 0.7f) + (y - 0.7f) * (y - 0.7f)) / 0.05f);
            heights[i][j] = hill1 + hill2;
        }
    }
}
// Function to draw the ground plane
void drawGround() {
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE - 1; ++i) {
        for (int j = 0; j < GRID_SIZE - 1; ++j) {
            float x0 = (i- (GRID_SIZE/2)) * CELL_SIZE;
            float z0 = (j- (GRID_SIZE/2)) * CELL_SIZE;
            float x1 = (i + 1- (GRID_SIZE/2)) * CELL_SIZE;
            float z1 = (j + 1- (GRID_SIZE/2)) * CELL_SIZE;
            
            glColor3f(0.0f, 0.8f, 0.0f); // Green color for the ground
            glVertex3f(x0, heights[i][j], z0);
            glVertex3f(x1, heights[i+1][j], z0);
            glVertex3f(x1, heights[i+1][j+1], z1);
            glVertex3f(x0, heights[i][j+1], z1);
        }
    }
    glEnd();
}
// Function to create a river
void createRiver() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        // River follows a sine wave path
        float riverX = (GRID_SIZE / 2.0f) * (sinf(i * 0.05f) + 0.7f);
        for (int j = 0; j < GRID_SIZE; ++j) {
            float dist = fabs(j - riverX);
            if (dist < RIVER_WIDTH) {
                heights[i][j] -= RIVER_DEPTH * expf(-dist * dist / (2 * RIVER_WIDTH * RIVER_WIDTH));
            }
        }
    }
}

// Function to draw the water in the river
void drawWater() {
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE - 1; ++i) {
        for (int j = 0; j < GRID_SIZE - 1; ++j) {
            float x0 = (i - HALF_GRID_SIZE) * CELL_SIZE;
            float z0 = (j - HALF_GRID_SIZE) * CELL_SIZE;
            float x1 = (i + 1 - HALF_GRID_SIZE) * CELL_SIZE;
            float z1 = (j + 1 - HALF_GRID_SIZE) * CELL_SIZE;
            
            // Check if the current cell is part of the river
            if (heights[i][j] < 0 || heights[i+1][j] < 0 || heights[i+1][j+1] < 0 || heights[i][j+1] < 0) {
                glColor3f(0.0f, 0.4f, 1.0f); // Blue color for the water
                glVertex3f(x0, -RIVER_DEPTH, z0);
                glVertex3f(x1, -RIVER_DEPTH, z0);
                glVertex3f(x1, -RIVER_DEPTH, z1);
                glVertex3f(x0, -RIVER_DEPTH, z1);
            }
        }
    }
    glEnd();
}

// Function to draw a simple polygon-style house
void drawHouse(float centerX, float centerY, float centerZ, float width, float height) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    // Draw the base of the house (a rectangular prism)
    glBegin(GL_QUADS);
    
    // Front face
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the base
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    
    // Back face
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    
    // Left face
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    
    // Right face
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    
    // Bottom face (optional, usually not visible)
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);
    
    // Top face (if needed, but typically covered by the roof)
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    
    glEnd();
    
    // Draw the roof (a prism with a triangular cross-section)
    glBegin(GL_TRIANGLES);
    
    // Front face of the roof
    glColor3f(0.5f, 0.35f, 0.05f); // Brown color for the roof
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);
    
    // Back face of the roof
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);
    
    glEnd();
    
    glBegin(GL_QUADS);
    
    // Left face of the roof
    glColor3f(0.5f, 0.35f, 0.05f);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);
    
    // Right face of the roof
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);
    
    glEnd();
}
// Function to set up the perspective projection
void setupPerspectiveCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   1.0,   // Near clipping plane
                   100.0); // Far clipping plane
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0, 10.0, 20.0,  // Eye position
              0.0, 1.0, 0.0,     // Point to look at
              0.0, 1.0, 0.0);    // Up vector
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
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
   //  Decide what to draw
   drawGround();
   drawForest(70,GRID_SIZE);
   createRiver();
   drawWater();
   drawHouse(8,0,10,3,2);
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
   Print("Angle=%d,%d",th,ph);
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
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Eli Jordan Forest Scene HW3");
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
   return 0;
}
