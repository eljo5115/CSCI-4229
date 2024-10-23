#include "drawObjects.h"
#include "CSCIx229.h"
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading

int emission  =   1;  // Emission intensity (%)

int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
// Array to store heights for the grid
float heights[GRID_SIZE][GRID_SIZE];

#define SLICES 32

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
void drawTrunk(GLuint texture) {
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

void drawPalmLeaf(double x, double y, double z, float r, int segments, float angleOffset, GLuint texture) {
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture);
    
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
