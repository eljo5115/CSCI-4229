#include "CSCIx229.h"

#ifndef drawObjects
#define drawObjects

typedef struct {
    float x1, x2, x3, x4;
    float z1,z2,z3,z4;
    float y1,y2,y3,y4;
    int type;
} quad;


#define FOREST_SEED 145
#define GREEN_SEED 100257 //Random between 0-10000 for hourglass, random > 100000 for oval
#define GRID_SIZE 50  // Number of grid cells along one axis
#define HALF_GRID_SIZE GRID_SIZE/2
#define CELL_SIZE 1.0f  // Size of each cell in the grid
// River parameters
#define RIVER_WIDTH 2.5f
#define RIVER_DEPTH 1.0f

#define FAIRWAY 0
#define ROUGH 1
#define GREEN 2
#define BUNKER 3
#define WATER 4

static void SphereVertex(double th,double ph);
static void ball(double x,double y,double z,double r);
void drawIcosahedron(GLuint texture);
void drawTrunk(GLuint texture);
void drawPalmLeaf(double x, double y, double z, float r, int segments, float angleOffset, GLuint texture);
void drawGround(GLuint texture);
void drawWater(GLuint texture);
void initializeHeights();
void createRiver();

void createFairway(int length);

quad** createGreen(float x, float y, float z, int rows, int columns, float bumpiness,float radiusX,float radiusZ);
void drawGreen(quad** quadArray, int rows, int columns);
void freeGreen(quad** quadArray, int rows);
int isInvalidQuad(quad q);
void drawTeeBox();
int isInsideShape(float x, float z,float a, float b ,float centerX, float centerZ, float radiusX, float radiusZ);
void drawFairway();

void drawQuad(quad q);
void createTeeBox(float width, float depth, float height,float bottomOffset);
void calculateNormalAndSet(quad q);
#endif