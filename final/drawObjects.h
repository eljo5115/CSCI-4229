#include "CSCIx229.h"

#ifndef drawObjects
#define drawObjects

typedef struct {
    float x1, x2, x3, x4;
    float z1,z2,z3,z4;
    float y1,y2,y3,y4;
    int type;
    bool hasTree;
    bool hasFlag;
} quad;

typedef struct {
    float x,y,z;
} Point3D;


#define FOREST_SEED 145
#define GREEN_SEED 100045 //Random between 0-10000 for hourglass, random > 100000 for oval
#define HOLE_SEED 13000 //Seed for hole gen
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

void SphereVertex(double th,double ph);
void ball(double x,double y,double z,double r);
void drawIcosahedron(GLuint texture);
void drawTrunk();
void drawPalmLeaf(double x, double y, double z, float r, int segments, float angleOffset);
void drawGround(GLuint texture);
void drawWater(GLuint texture);
void initializeHeights();
void createRiver();
int generateRandomInt(int min, int max);
quad** createFairway(Point3D greenLocation, int width,  int holeLength, int roughWidth);
void drawFairway(quad** quadArray, int rows, int columns);

void drawTree(float x, float y, float z,float height);
void drawPalmTreeLeaves(double x, double y, double z, int numLeaves, float leafRadius, int leafSegments) ;

quad** createGreen(float x, float y, float z, int rows, int columns,float radiusX,float radiusZ);
void drawGreen(quad** quadArray, int rows, int columns);
int isInvalidQuad(quad q);
int isInsideShape(float x, float z,float a, float b ,float centerX, float centerZ, float radiusX, float radiusZ);

void createTeeBox(float width, float depth, float height,float bottomOffset);
void drawTeeBox();

Point3D catmullRomSpline(Point3D p0, Point3D p1, Point3D p2, Point3D p3, float t);

void freeQuadArray(quad** quadArray, int rows);
void drawQuad(quad q);
void calculateNormalAndSet(quad q);
Point3D* generateRandomControlPoints(int numPoints, float width, float length);
#endif