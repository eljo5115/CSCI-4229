#include "drawObjects.h"

#include "CSCIx229.h"
#include "perlin.h"
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading

int emission  =   1;  // Emission intensity (%)

int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
// Array to store heights for the grid
float heights[GRID_SIZE][GRID_SIZE];

#define SLICES 32

typedef struct {
    Point3D location;
    float a,b;
    float centerX,centerZ;
    float radiusX,radiusZ;
} greenData;

greenData sharedGreen;

/*
 *  Draw vertex in polar coordinates with normal
 */
void SphereVertex(double th,double ph)
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
void ball(double x,double y,double z,double r)
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
         SphereVertex(th,ph);
         SphereVertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}

// Define the constant for the golden ratio
#define GOLDEN_RATIO 1.61803398875
// Draw an icosahedron with normals
void drawIcosahedron(GLuint texture) {
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
    glBindTexture(GL_TEXTURE_2D,texture);
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
    // glBindTexture(GL_TEXTURE_2D,texture);

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
    // glBindTexture(GL_TEXTURE_2D,texture);
    
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
            SphereVertex(th, ph);

            calculateNormal(th, ph + inc, &nx, &ny, &nz);
            glNormal3f(nx, ny, nz);
            glTexCoord2f(1,1);
            SphereVertex(th, ph + inc);
        }
        glEnd();
    }
    // Undo transformations
    glPopMatrix();
}
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
        drawPalmLeaf(x,y,z,leafRadius, leafSegments, -PI / 2);  // Draw leaf
        glPopMatrix();
    }
}
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
    drawPalmTreeLeaves(0,0,0,   12,0.8,20);  // 12 leaves, radius 0.8, 20 segments per leaf
    drawPalmTreeLeaves(0,0.2f,0,    10, 0.5,20);  // 10 leaves, radius 0.5, 20 segments per leaf

    glPopMatrix();
    

}
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
void drawGround(GLuint texture) {
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
    glBindTexture(GL_TEXTURE_2D,texture);

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

// Function to draw the water in the river
void drawWater(GLuint texture) {
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
    glBindTexture(GL_TEXTURE_2D,texture);
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
void renderNormalDebug(quad q) {
    float center[3] = {(q.x1 + q.x2 + q.x3 + q.x4) / 4.0f,
                       (q.y1 + q.y2 + q.y3 + q.y4) / 4.0f,
                       (q.z1 + q.z2 + q.z3 + q.z4) / 4.0f};
    float normal[3];
    
    normal[0] = q.x1+q.x2+q.x3+q.x4;
    normal[1] = q.y1+q.y2+q.y3+q.y4;
    normal[2] = q.z1+q.z2+q.z3+q.z4;
    normalize(normal);

    // Draw a line from the center of the quad along the normal
    glBegin(GL_LINES);
    glColor3ub(250,0,0);
    glVertex3f(center[0], center[1], center[2]);
    glVertex3f(center[0] + normal[0] * 0.5f, center[1] + normal[1] * 0.5f, center[2] + normal[2] * 0.5f);
    glEnd();
}
// Function to calculate and set the normal for a quad
void calculateNormalAndSet(quad q) {
    float normal[3];

    normal[0] = q.x1+q.x2+q.x3+q.x4;
    normal[1] = q.y1+q.y2+q.y3+q.y4;
    normal[2] = q.z1+q.z2+q.z3+q.z4;
    normalize(normal);
    // if(normal[1] < 0){
    //     normal[0] = -normal[0];
    //     normal[1] = -normal[1];
    //     normal[2] = -normal[2];
    // }
    if(_DEBUG){
        renderNormalDebug(q);
    }
    glNormal3f(normal[0], normal[1], normal[2]);
}
void drawCylinder(float radius, float height) {
    // Draw the side of the cylinder
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SLICES; ++i) {
        float angle = 2 * PI * i / SLICES;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        
        // Normals for cylinder sides
        glNormal3f(cos(angle), 0.0f, sin(angle));
        
        // Vertex at bottom of cylinder
        glVertex3f(x, 0.0f, z);
        // Vertex at top of cylinder
        glVertex3f(x, height, z);
    }
    glEnd();

    // Draw the bottom circle
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= SLICES; ++i) {
        float angle = 2 * PI * i / SLICES;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

void drawFlag(float x, float y, float z, float stickHeight, float flagWidth, float flagHeight) {
    // Translate the flag to the specified position
    glPushMatrix();
    glTranslatef(x, y, z);

    // Set the color for the flag stick
    glColor3f(0.5f, 0.5f, 0.5f);
    
    // Draw the stick
    drawCylinder(0.02f, stickHeight);

    // Set the color for the flag
    glColor3f(1.0f, 0.0f, 0.0f);

    // Draw the flag as a single triangle
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, stickHeight, 0.0f);                // Base point attached to the stick
    glVertex3f(flagWidth, stickHeight - flagHeight, 0.0f); // Top corner of the flag
    glVertex3f(flagWidth, stickHeight, 0.0f);              // Tip of the flag
    glEnd();

    // Restore the previous transformation state
    glPopMatrix();
}
/*
Draws the given quad struct (q)
Can be called in display func
Params: 
q - quad to draw
Returns:
None
*/
void drawQuad(quad q) {
    //make normals
    calculateNormalAndSet(q);
    //switch based on type of quad
    switch(q.type){
        case FAIRWAY:
            glColor3ub(24,230,13);
            break;
        case ROUGH:
            glColor3ub(20,180,30);
            break;
        case GREEN:
            glColor3ub(47,250,60);
            break;
        case BUNKER:
            glColor3ub(255,242,122);
            break;
        default:
            glColor3ub(0,0,255);
            break;
    }
    //draw quad
    glBegin(GL_QUADS);
    glVertex3f(q.x1, q.y1, q.z1);
    glVertex3f(q.x2, q.y2, q.z2);
    glVertex3f(q.x3, q.y3, q.z3);
    glVertex3f(q.x4, q.y4, q.z4);
    glEnd();
}

/*
Function to create a plateaued surface with sloped sides
Misnomered function, DRAWS tee box
*/
void createTeeBox(float width, float depth, float height, float bottomOffset) {
    // drawing a box
    // top first as that's the easiest
    quad q;

    q.y1 = q.y2 = q.y3 = q.y4 = height; 
    q.x1 = width;
    q.z1 = depth;

    q.x2 = width;
    q.z2 = -depth;

    q.x3 = -width;
    q.z3 = -depth;

    q.x4 = -width;
    q.z4 = depth;
    q.type = FAIRWAY;
    drawQuad(q);
    q.type = ROUGH;
    //draw left side
    // points on top
    q.y1 = q.y2 = height; 
    q.x1 = q.x2 = -width;
    q.z1 = depth; q.z2 = -depth;
    //points on bottom
    //offset to make slope
    q.y3 = q.y4 = 0;

    q.x3 = q.x4 = -width-bottomOffset;
    q.z4 = depth + bottomOffset;
    q.z3 = -depth - bottomOffset;
    drawQuad(q);
    //draw right side
    //top points
    q.y1 = q.y2 = height; 
    q.x1 = q.x2 = width;
    q.z1 = depth; q.z2 = -depth;

    //bottom points
    q.y3 = q.y4 = 0;
    q.x3 = q.x4 = width + bottomOffset;
    q.z4 = depth + bottomOffset;
    q.z3 = -depth - bottomOffset;
    drawQuad(q);
    //draw front side
    //top points
    q.y1 = q.y2 = height; 
    q.x1 = width; q.x2 = -width;
    q.z1 =q.z2 = depth;

    //bottom points
    q.y3 = q.y4 = 0;
    q.x4 = width + bottomOffset;
    q.x3 = -width - bottomOffset;
    q.z4=q.z3 = depth + bottomOffset;
    drawQuad(q);
    //draw back side
    //top points
    q.y1 = q.y2 = height;
    q.x1 = width; q.x2 = -width;
    q.z1 = q.z2 = -depth;
    //bottom points 
    q.y3 = q.y4 = 0;
    q.x4 = width + bottomOffset;
    q.x3 = -width - bottomOffset;
    q.z4 =q.z3 = -depth - bottomOffset;
    drawQuad(q);
}
// Generate random float between min and max
float randomFloat(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}
// checks quad for invalid values
int isInvalidQuad(quad q) {
    return q.x1 == -1 && q.y1 == -1 && q.z1 == -1;
}

// Check if a point (x, z) is inside a given elliptical area
int isInsideShape(float x, float z,float a,float b ,float centerX, float centerZ, float radiusX, float radiusZ) {
    float dx = x - centerX;
    float dz = z - centerZ;
    return (a * (dx * dx) / (radiusX * radiusX) + b * (dz * dz) / (radiusZ * radiusZ)) <= 1.0f;
}

int isInsideGreen(float x, float z,float a,float b ,float centerX, float centerZ, float radiusX, float radiusZ){
        // Calculate the delta values
    float dx = x - centerX;
    float dz = z - centerZ;

    // Check if inside the ellipse-like boundary
    int insideEllipse = (a * (dx * dx) / (radiusX * radiusX) + b * (dz * dz) / (radiusZ * radiusZ)) <= 1.0f;

    // Restrict check to within predefined rectangular bounds
    int withinBounds = (x >= centerX - radiusX) && (x <= centerX + radiusX) &&
                       (z >= centerZ - radiusZ) && (z <= centerZ + radiusZ);

    // Return true only if both conditions are satisfied
    return insideEllipse && withinBounds;
}
// Function to return true based on a given probability
bool randomProbability(float probability) {
    if (probability < 0.0f || probability > 1.0f) {
        // Handle invalid probability values
        fprintf(stderr, "Probability must be between 0 and 1\n");
        return false;
    }

    // Generate a random float between 0 and 1
    float randomValue = (float)rand() / RAND_MAX;

    // Return true if the random value is less than the probability
    return randomValue < probability;
}

int generateRandomInt(int min, int max){
   return rand() % (max - min + 1) + min;
}
/*
Function to create a green with bumps, hills, and a predefined shape
Params: 
x,y,z - center of green
rows, columns - size of height map
bumpiness - float for random change in elevation
radiusX, radiusZ - floats for the shape equation
Returns:
quad[][] - 2d array of quads (struct) that have the absolute position of the green
*/
quad** createGreen(float x, float y, float z, int rows, int columns, float radiusX, float radiusZ) {
    // srand(GREEN_SEED);
    sharedGreen.radiusX = radiusX;
    sharedGreen.radiusZ = radiusZ;
    bool flagPlaced = false;
    quad** quadArray = (quad**)malloc(rows * sizeof(quad*));
    for (int i = 0; i < rows; ++i) {
        quadArray[i] = (quad*)malloc(columns * sizeof(quad));
    }

    float xOffset = 0.8f; // Adjust as needed for quad size
    float zOffset = 0.8f; // Adjust as needed for quad size
    sharedGreen.centerX = x;
    sharedGreen.centerZ = z;
    sharedGreen.a = randomFloat(-0.5,0.7);

    if((sharedGreen.a < 0.1 && sharedGreen.a > 0) || sharedGreen.a<0 ){
        sharedGreen.b = randomFloat(0.2,0.9);
    }else{
        sharedGreen.b = randomFloat(-0.5,0.7);
    }
    // printf("Random a: %f, Random b: %f",a,b);
    float heightMap[rows+1][columns+1];
    initializePermutations(); //initialize permutations for the perlin noise function
    float scale = 0.1;
    // Generate heights for the grid vertices
    for (int i = 0; i <= rows; i++) {
        for (int j = 0; j <= columns; j++) {
            heightMap[i][j] = y+ perlinNoise(j*scale ,i*scale);
            // printf("x: %d, y:%d = %f\n",i,j,heightMap[i][j]);
        }
    }

    // Initialize and populate quadArray ensuring shared heights
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            quad q;
            float xStart = x + j * xOffset - (columns * xOffset) / 2;
            float zStart = z + i * zOffset - (rows * zOffset) / 2;

            bool inside = isInsideShape(xStart + xOffset / 2, zStart + zOffset / 2, sharedGreen.a, sharedGreen.b, sharedGreen.centerX, sharedGreen.centerZ, radiusX, radiusZ);

            // Check if the quad is within the shape
            if (!inside && (
                isInsideShape(xStart + xOffset / 2, zStart + zOffset / 2, sharedGreen.a, sharedGreen.b, sharedGreen.centerX, sharedGreen.centerZ, radiusX + xOffset, radiusZ) ||
                isInsideShape(xStart + xOffset / 2, zStart + zOffset / 2, sharedGreen.a, sharedGreen.b, sharedGreen.centerX, sharedGreen.centerZ, radiusX, radiusZ + zOffset)) )
                {                
                // Valid quad coordinates
                q.x1 = xStart;
                q.y1 = heightMap[i][j];
                q.z1 = zStart;

                q.x2 = xStart + xOffset;
                q.y2 = heightMap[i][j+1];
                q.z2 = zStart;

                q.x3 = xStart + xOffset;
                q.y3 = heightMap[i+1][j+1];
                q.z3 = zStart + zOffset;

                q.x4 = xStart;
                q.y4 = heightMap[i+1][j];
                q.z4 = zStart + zOffset;
                q.type = ROUGH;
                q.hasFlag = false;
                
                // printf("Quad[%d][%d] Heights:\n", i, j);
                // printf("  (%f) -> (%f)\n", q.y1, q.y2);
                // printf("  (%f) -> (%f)\n", q.y4, q.y3);
            } else if(inside) {
                                // Valid quad coordinates
                q.x1 = xStart;
                q.y1 = heightMap[i][j];
                q.z1 = zStart;

                q.x2 = xStart + xOffset;
                q.y2 = heightMap[i][j+1];
                q.z2 = zStart;

                q.x3 = xStart + xOffset;
                q.y3 = heightMap[i+1][j+1];
                q.z3 = zStart + zOffset;

                q.x4 = xStart;
                q.y4 = heightMap[i+1][j];
                q.z4 = zStart + zOffset;
                q.type = GREEN;
                q.hasFlag = false;
                //flag not yet placed
                if(!flagPlaced){
                    //random chance of quad having flag
                    if(randomProbability(0.3)){
                        //give quad flag
                        q.hasFlag = true;
                        //flag has been placed
                       flagPlaced = true;
                    }
                }
            }else{
                // Mark as invalid
                q.x1 = -1;
                q.y1 = -1;
                q.z1 = -1;
            }

            quadArray[i][j] = q;
        }
    }

    return quadArray;
}
// A function to free the allocated memory for quads
void freeQuadArray(quad** quadArray, int rows) {
    for (int i = 0; i < rows; ++i) {
        free(quadArray[i]);
    }
    free(quadArray);
}

/*
Draws all quads in the given quadArray
Params:
quadArray[][] - 2d array of quads (struct) that define the green
rows,columns - size of quadArray
Returns:
None
*/
void drawGreen(quad** quadArray, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (isInvalidQuad(quadArray[i][j]))continue;
            drawQuad(quadArray[i][j]);
            if(quadArray[i][j].hasFlag){
                drawFlag(quadArray[i][j].x1,quadArray[i][j].y1,quadArray[i][j].z1,3,3,4);
            }
        }
    }
}

// Catmull-Rom spline interpolation
Point3D catmullRomSpline(Point3D p0, Point3D p1, Point3D p2, Point3D p3, float t) {
    // Catmull-Rom parameters
    float t2 = t * t;
    float t3 = t2 * t;

    Point3D result;
    result.x = 0.5 * ((2*p1.x) + (-p0.x + p2.x) * t + 
                      (2*p0.x - 5*p1.x + 4*p2.x - p3.x) * t2 +
                      (-p0.x + 3*p1.x - 3*p2.x + p3.x) * t3);
    result.y = 0.5 * ((2*p1.y) + (-p0.y + p2.y) * t + 
                      (2*p0.y - 5*p1.y + 4*p2.y - p3.y) * t2 +
                      (-p0.y + 3*p1.y - 3*p2.y + p3.y) * t3);
    result.z = 0.5 * ((2*p1.z) + (-p0.z + p2.z) * t + 
                      (2*p0.z - 5*p1.z + 4*p2.z - p3.z) * t2 +
                      (-p0.z + 3*p1.z - 3*p2.z + p3.z) * t3);
    return result;
}
void initializeControlPoints(Point3D* controlPoints, int numControlPoints, int holeLength, int width) {
    for (int i = 1; i < numControlPoints - 1; i++) {
        controlPoints[i] = (Point3D){
            randomFloat(-width / 2.0, width / 2.0),
            0.0,
            randomFloat((i * holeLength) / numControlPoints, ((i+1) * holeLength) / numControlPoints)
        };
    }
}
// Main fairway creation
quad** createFairway(Point3D greenLocation, int width, int holeLength, int roughWidth) {
    // srand(HOLE_SEED);
    
    int totalWidth = width + 2 * roughWidth;
    
    // Allocate memory for quads
    quad** quadArray = (quad**)malloc(totalWidth * sizeof(quad*));
    for (int i = 0; i < totalWidth; ++i) {
        quadArray[i] = (quad*)malloc(holeLength * sizeof(quad));
    }
    float heightMap[holeLength+(int)floor(sharedGreen.radiusZ)+1][totalWidth+1];
    initializePermutations(); //initialize permutations for the perlin noise function
    float scale = 0.1;
    float heightScale = 2; // fairway noise is bigger
    // Generate heights for the grid vertices
    for (int i = 0; i <= holeLength+(int)floor(sharedGreen.radiusZ); i++) {
        for (int j = 0; j <= totalWidth; j++) {
            heightMap[i][j] = perlinNoise(j*scale ,i*scale) * heightScale;
            // printf("x: %d, y:%d = %f\n",i,j,heightMap[i][j]);
        }
    }

    int widthMap[holeLength+1]; // used to randomize fairway width through path
    widthMap[0] = width;
    for(int i = 1; i <= holeLength; i++){
        int posChange = generateRandomInt(0,width);
        int negChange = generateRandomInt(-width,0);
        // if(widthMap[i-1]+posChange < totalWidth - width){
        //     widthMap[i] = widthMap[i-1] + posChange;
        // }else if(widthMap[i-1]+negChange < 0){
        //     widthMap[i] = widthMap[i-1] + negChange;
        // }
        widthMap[i] = -Cos(i)*width; // set width of fairway
    }


    float xOffset = 1.0f; // Distance between quad vertices in x
    float zOffset = 1.0f; // Distance between quad vertices in z

    // Generate a "flat" grid between (0, 0, 0) and greenLocation
    for (int x = 0; x < totalWidth; ++x) {
        for (int z = 0; z < holeLength; ++z) {
            quad q;

            q.x1 = x * xOffset - (totalWidth/2);
            q.y1 = heightMap[x][z];  // Flat surface
            q.z1 = z * zOffset;

            q.x2 = (x + 1) * xOffset - (totalWidth/2);
            q.y2 = heightMap[x+1][z];  // Flat surface
            q.z2 = z * zOffset;

            q.x3 = (x + 1) * xOffset - (totalWidth/2);
            q.y3 = heightMap[x+1][z+1];  // Flat surface
            q.z3 = (z + 1) * zOffset;

            q.x4 = x * xOffset - (totalWidth/2);
            q.y4 = heightMap[x][z+1];  // Flat surface
            q.z4 = (z + 1) * zOffset;
            if(isInsideShape(q.x1,q.z1,sharedGreen.a,sharedGreen.b,sharedGreen.centerX,sharedGreen.centerZ,sharedGreen.radiusX,sharedGreen.radiusZ) 
            && (q.z1 > holeLength-sharedGreen.radiusZ || q.z3 > holeLength-sharedGreen.radiusZ) 
            && (q.x1 > sharedGreen.radiusX || q.x2 < sharedGreen.radiusX)
            )
            {
                q.x1 = -1;
                q.y1 = -1;
                q.z1 = -1;
            }
            if( (x < widthMap[z] || x >= totalWidth - widthMap[z]) && q.x1 != -1){
                q.type = ROUGH;
                q.hasTree = false;
                if(randomProbability(0.05)){
                    q.hasTree = true;
                }
            }else{
                q.hasTree = false;
                q.type = FAIRWAY;
            }
            quadArray[x][z] = q;
        }
    }
    int numBunkerAttempts = generateRandomInt(1,5);
    //try to create a bunker a number of times
    for(int i = 0; i<numBunkerAttempts; i++){
        int bunkerX = generateRandomInt(0,totalWidth);
        int bunkerZ = generateRandomInt(0,holeLength);
        int bunkerSize = generateRandomInt(10,30);
        //loop back through array to find rough and create bunkers
        if(randomProbability(0.5)) // probability of having a bunker
        {
            for(int x = bunkerX-bunkerSize; x < totalWidth && x > 0 && x < bunkerX+bunkerSize; x++){
                for(int z = bunkerZ-bunkerSize; z<holeLength && z >= 0 && z<bunkerZ + bunkerSize; z++){
                        quadArray[x][z].type = BUNKER;

                }
            }
        }
    }
    return quadArray;
}

void drawFairway(quad** quadArray, int rows, int columns){
    for(int i = 0; i<columns; i++){
        for(int j=0;j<rows;j++){
            if(!isInvalidQuad(quadArray[i][j])) drawQuad(quadArray[i][j]);
            if(quadArray[i][j].hasTree){
                drawTree(quadArray[i][j].x1,quadArray[i][j].y1,quadArray[i][j].z1,8);
            }
        }
    }
}

// Generate random control points for a spline
Point3D* generateRandomControlPoints(int numPoints, float width, float length) {
    Point3D* controlPoints = (Point3D*)malloc(numPoints * sizeof(Point3D));
    if (!controlPoints) {
        perror("Failed to allocate memory for control points");
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand(HOLE_SEED);

    // Generate control points
    for (int i = 0; i < numPoints; i++) {
        controlPoints[i].x = randomFloat(-width / 2.0, width / 2.0);
        controlPoints[i].y = 0.0; // Assuming flat for control points, modify for height variations
        controlPoints[i].z = randomFloat(0, length);
        printf("Control Point %d: (%f, %f, %f)\n", i, controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
    }

    return controlPoints;
}