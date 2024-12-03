#include "CSCIx229.h"

// Normalize a vector
void normalize(float v[3]) {
    float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    for (int i = 0; i < 3; i++) {
        v[i] /= length;
    }
}
// Compute the cross product of two vectors
void crossProduct(float v1[3], float v2[3], float result[3]) {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
void normalizeVertex(float* vertex) {
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

float interpolate(float p0, float p1, float t) {
    return (1 - t) * p0 + t * p1;
}