#include "CSCIx229.h"
#ifndef drawObjects
#define drawObjects

#define FOREST_SEED 136
#define GRID_SIZE 50  // Number of grid cells along one axis
#define HALF_GRID_SIZE GRID_SIZE/2
#define CELL_SIZE 1.0f  // Size of each cell in the grid
// River parameters
#define RIVER_WIDTH 2.5f
#define RIVER_DEPTH 1.0f
static void Vertex(double th,double ph);
static void ball(double x,double y,double z,double r);
void drawIcosahedron(GLuint texture);
void drawTrunk(GLuint texture);
void drawPalmLeaf(double x, double y, double z, float r, int segments, float angleOffset, GLuint texture);
void drawGround(GLuint texture);
void drawWater(GLuint texture);
void initializeHeights();
void createRiver();
#endif