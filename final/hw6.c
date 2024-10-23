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

int _DEBUG = 0; //DEBUG bool, set 1 for console print out

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
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   1;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   3;  // Elevation of light
const char* text[] = {"Ortho","Top-Down Perspective","First Person"};


//textures
unsigned int texture[5];

#define FOREST_SEED 136
#define GRID_SIZE 50  // Number of grid cells along one axis
#define HALF_GRID_SIZE GRID_SIZE/2
#define CELL_SIZE 1.0f  // Size of each cell in the grid
// River parameters
#define RIVER_WIDTH 2.5f
#define RIVER_DEPTH 1.0f
#define SLICES 32

#define VELOCITY 0.5f

// Normalize a vector
void normalize(GLfloat v[3]) {
    GLfloat length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    for (int i = 0; i < 3; i++) {
        v[i] /= length;
    }
}

// Compute the cross product of two vectors
void crossProduct(GLfloat v1[3], GLfloat v2[3], GLfloat result[3]) {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
void normalizeVertex(GLfloat* vertex) {
    float length = sqrt(vertex[0] * vertex[0] + vertex[1] * vertex[1] + vertex[2] * vertex[2]);
    vertex[0] /= length;
    vertex[1] /= length;
    vertex[2] /= length;
}
// Function to calculate the normal at a given latitude and longitude
void calculateNormal(float th, float ph, float* nx, float* ny, float* nz) {
    // Convert angles from degrees to radians
    float th_rad = th * PI / 180.0;
    float ph_rad = ph * PI / 180.0;

    // Calculate the normal using spherical coordinates
    *nx = cos(th_rad) * sin(ph_rad);
    *ny = sin(th_rad) * sin(ph_rad);
    *nz = cos(ph_rad);
}
/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}
/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}
// Define the constant for the golden ratio
#define GOLDEN_RATIO 1.61803398875
// Draw an icosahedron with normals
void drawIcosahedron() {
        //set materials
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    // Vertices of the icosahedron
    GLfloat vertices[12][3] = {
        { 1, GOLDEN_RATIO, 0}, {-1, GOLDEN_RATIO, 0}, { 1,-GOLDEN_RATIO, 0}, {-1,-GOLDEN_RATIO, 0},
        { 0, 1, GOLDEN_RATIO}, { 0,-1, GOLDEN_RATIO}, { 0, 1,-GOLDEN_RATIO}, { 0,-1,-GOLDEN_RATIO},
        { GOLDEN_RATIO, 0, 1}, {-GOLDEN_RATIO, 0, 1}, { GOLDEN_RATIO, 0,-1}, {-GOLDEN_RATIO, 0,-1}
    };

    for(int i = 0; i<12; i++){
        normalizeVertex(vertices[i]);
    }
    // Faces of the icosahedron (each row represents a triangular face)
    int faces[20][3] = {
        {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
        {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
        {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
        {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        GLfloat v1[3], v2[3], normal[3];

        // First vertex
        GLfloat* p0 = vertices[faces[i][0]];
        // Second vertex
        GLfloat* p1 = vertices[faces[i][1]];
        // Third vertex
        GLfloat* p2 = vertices[faces[i][2]];

        // Edge vectors
        v1[0] = p1[0] - p0[0];
        v1[1] = p1[1] - p0[1];
        v1[2] = p1[2] - p0[2];

        v2[0] = p2[0] - p0[0];
        v2[1] = p2[1] - p0[1];
        v2[2] = p2[2] - p0[2];

        // Cross product to get the normal
        crossProduct(v1, v2, normal);

        // Normalize the normal
        normalize(normal);

        // Set the normal for the face
        glNormal3fv(normal);

        // Draw the face
        glTexCoord2f(0,0);glVertex3fv(p0);
        glTexCoord2f(0.5,1);glVertex3fv(p1);
        glTexCoord2f(1,0);glVertex3fv(p2);
    }
    glEnd();
}
// Draw the tree trunk as a cylinder
void drawTrunk() {
    //set materials
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[0]);

    float radius = 0.1f;
    float height = 2.0f;

    // Draw the side of the cylinder
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SLICES; i++) {
        float angle = 2 * M_PI * i / SLICES;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float nx = cos(angle); // Normal x component
        float nz = sin(angle); // Normal z component
        
        // Set normal
        glNormal3f(nx, 0.0f, nz);
        
        // Set vertices
        glTexCoord2f(0,0);glVertex3f(x, 0.0f, z);
        glTexCoord2f(1,1);glVertex3f(x, height, z);
    }
    glEnd();

    // Draw the top circle
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal for the top circle
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= SLICES; i++) {
        float angle = 2 * M_PI * i / SLICES;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        glVertex3f(x, height, z);
    }
    glEnd();
}

void drawPalmLeaf(double x, double y, double z, float r, int segments, float angleOffset) {
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    
    // Save transformation
    glPushMatrix();
    // Offset, scale, and rotate
    glTranslated(x, y-r, z);
    glScaled(r, r, r);

    glColor3f(0.1, 0.8, 0);

    // Bands of latitude
    for (int ph = 0; ph < 80; ph += inc) {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 45; th += 2 * inc) {
            float nx, ny, nz;
            calculateNormal(th, ph, &nx, &ny, &nz);
            glNormal3f(nx, ny, nz);
            glTexCoord2f(0,0);
            Vertex(th, ph);

            calculateNormal(th, ph + inc, &nx, &ny, &nz);
            glNormal3f(nx, ny, nz);
            glTexCoord2f(1,1);
            Vertex(th, ph + inc);
        }
        glEnd();
    }
    // Undo transformations
    glPopMatrix();
}

void drawPalmTreeLeaves(double x, double y, double z, int numLeaves, float leafRadius, int leafSegments) {
    // x,y,z is central point (top of trunk)
    float angInc = 360.0/numLeaves;
    for (int i = 0; i < numLeaves; i++) {
        float angleOffset = 2 * PI * i / numLeaves;
        // double Lx;
        // double Lz; 
        // Lx = leafRadius * Cos(i*angInc);
        // Lz = leafRadius * Sin(i*angInc);
        glPushMatrix();
        glRotatef(angleOffset *180/PI, 0, 1, 0);  // Rotate each leaf around y-axis
        drawPalmLeaf(x,y,z,leafRadius, leafSegments, -PI / 2);  // Draw leaf
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
    drawTrunk();
    
    // Draw leaves as icosahedrons
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    
    // Translate to the top of the trunk
    glTranslatef(0,2,0);
    //glRotated();
    // Draw leaves as palm leaves
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    drawPalmTreeLeaves(0,0,0,12, 0.8,20);  // 12 leaves, radius 0.8, 20 segments per leaf
    drawPalmTreeLeaves(0,0.2f,0,10, 0.5,20);  // 10 leaves, radius 0.5, 20 segments per leaf

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
    //set materials
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);

    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE - 1; ++i) {
        for (int j = 0; j < GRID_SIZE - 1; ++j) {
            float x0 = (i - (GRID_SIZE / 2)) * CELL_SIZE;
            float z0 = (j - (GRID_SIZE / 2)) * CELL_SIZE;
            float x1 = ((i + 1) - (GRID_SIZE / 2)) * CELL_SIZE;
            float z1 = ((j + 1) - (GRID_SIZE / 2)) * CELL_SIZE;

            float p0[3] = {x0, heights[i][j], z0};
            float p1[3] = {x1, heights[i + 1][j], z0};
            float p2[3] = {x1, heights[i + 1][j + 1], z1};
            float p3[3] = {x0, heights[i][j + 1], z1};

            // Calculate vectors for the quad surface
            float v1[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};
            float v2[3] = {p3[0] - p0[0], p3[1] - p0[1], p3[2] - p0[2]};

            // Cross product to get the normal
            float normal[3];
            crossProduct(v1, v2, normal);

            // Check normal is facing up, flip if necessary
            if (normal[1] < 0) {
                normal[0] = -normal[0];
                normal[1] = -normal[1];
                normal[2] = -normal[2];
            }
            // Normalize the normal vector
            normalize(normal);

            if (_DEBUG){
                printf("Ground normal: %f, %f, %f\n", normal[0],normal[1],normal[2]);
            }
            // Set normal for the quad
            glNormal3fv(normal);

            // Set color and vertices for the quad
            glColor3f(0.0f, 0.8f, 0.0f); // Green color for the ground
            glTexCoord2f(0,0);glVertex3fv(p0);
            glTexCoord2f(1,0);glVertex3fv(p1);
            glTexCoord2f(0,1);glVertex3fv(p2);
            glTexCoord2f(1,1);glVertex3fv(p3);
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
                float decrease = RIVER_DEPTH * expf(-dist * dist / (2 * RIVER_WIDTH * RIVER_WIDTH));
                
                // Limit the change to the height
                if (heights[i][j] - decrease >= -RIVER_DEPTH*3) {
                    heights[i][j] -= decrease;
                }

                if(_DEBUG == 1){
                    printf("%f\n",heights[i][j]);
                }
            }
        }
    }

}

// Function to draw the water in the river
void drawWater() {
    //set materials
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE - 1; ++i) {
        for (int j = 0; j < GRID_SIZE - 1; ++j) {
            float x0 = (i - HALF_GRID_SIZE) * CELL_SIZE;
            float z0 = (j - HALF_GRID_SIZE) * CELL_SIZE;
            float x1 = ((i + 1) - HALF_GRID_SIZE) * CELL_SIZE;
            float z1 = ((j + 1) - HALF_GRID_SIZE) * CELL_SIZE;
            
            // Check if the current cell is part of the river
            if (heights[i][j] < 0 || heights[i+1][j] < 0 || heights[i+1][j+1] < 0 || heights[i][j+1] < 0) {
                // Set the normal pointing up
                glNormal3f(0.0f, 1.0f, 0.0f);
                
                // Set color for the water
                glColor3f(0.0f, 0.4f, 1.0f); // Blue color for the water
                
                // Draw the quad
                glTexCoord2f(0,0);glVertex3f(x0, -RIVER_DEPTH, z0);
                glTexCoord2f(1,0);glVertex3f(x1, -RIVER_DEPTH, z0);
                glTexCoord2f(0,1);glVertex3f(x1, -RIVER_DEPTH, z1);
                glTexCoord2f(1,1);glVertex3f(x0, -RIVER_DEPTH, z1);
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

    // Front face normals
    glNormal3f(0.0f, 0.0f, -1.0f);
    // Front face
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the base
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);

    // Back face normals
    glNormal3f(0.0f, 0.0f, 1.0f);
    // Back face
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);

    // Left face normals
    glNormal3f(-1.0f, 0.0f, 0.0f);
    // Left face
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);

    // Right face normals
    glNormal3f(1.0f, 0.0f, 0.0f);
    // Right face
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);

    // Bottom face normals
    glNormal3f(0.0f, -1.0f, 0.0f);
    // Bottom face (optional, usually not visible)
    glVertex3f(centerX - halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY, centerZ + halfWidth);

    // Top face normals
    glNormal3f(0.0f, 1.0f, 0.0f);
    // Top face (if needed, but typically covered by the roof)
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);

    glEnd();

    // Draw the roof (a prism with a triangular cross-section)
    glBegin(GL_TRIANGLES);

    float v1[3], v2[3], normal[3];

    // Roof front face normal
    v1[0] = halfWidth; v1[1] = 0; v1[2] = 0;
    v2[0] = 0; v2[1] = halfHeight; v2[2] = 0;
    crossProduct(v1, v2, normal);
    normalize(normal);
    glNormal3fv(normal);

    // Front face of the roof
    glColor3f(0.5f, 0.35f, 0.05f); // Brown color for the roof
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);

    // Roof back face normal
    v1[0] = halfWidth; v1[1] = 0; v1[2] = 0;
    v2[0] = 0; v2[1] = halfHeight; v2[2] = 0;
    crossProduct(v1, v2, normal);
    normalize(normal);
    glNormal3fv(normal);

    // Back face of the roof
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);

    glEnd();

    glBegin(GL_QUADS);

    // Roof left face normal
    v1[0] = 0; v1[1] = halfHeight; v1[2] = 0;
    v2[0] = 0; v2[1] = halfHeight; v2[2] = halfWidth * 2;
    crossProduct(v1, v2, normal);
    normalize(normal);
    glNormal3fv(normal);

    // Left face of the roof
    glColor3f(0.5f, 0.35f, 0.05f);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX - halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);

    // Roof right face normal
    v1[0] = 0; v1[1] = halfHeight; v1[2] = 0;
    v2[0] = 0; v2[1] = halfHeight; v2[2] = halfWidth * 2;
    crossProduct(v1, v2, normal);
    normalize(normal);
    glNormal3fv(normal);

    // Right face of the roof
    glVertex3f(centerX + halfWidth, centerY + height, centerZ - halfWidth);
    glVertex3f(centerX + halfWidth, centerY + height, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ + halfWidth);
    glVertex3f(centerX, centerY + height + halfHeight, centerZ - halfWidth);

    glEnd();
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
      ball(Position[0],Position[1],Position[2] , 1.0);
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
   drawForest(60,GRID_SIZE-5);
   createRiver();
   drawWater();
   drawHouse(8,0,10,3,2);
   drawGround();
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
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Eli Jordan Forest Scene HW6 Textures");
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
//    //Load textures
   texture[0] = LoadTexBMP("textures/trunk.bmp");
   texture[1] = LoadTexBMP("textures/grass.bmp");
   texture[2] = LoadTexBMP("textures/water.bmp");
//    texture[3] = LoadTexBMP("brick.bmp");
//    texture[4] - LoadTexBMP("leaves.bmp");

   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
