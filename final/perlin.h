#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef perlin
#define perlin

#define PERMUTATION_SIZE 40

// Simple Perlin Noise Function Declaration
float perlinNoise(float x, float y);

// Helper functions for Perlin noise
float lerp(float a, float b, float t);
float fade(float t);
float grad(int hash, float x, float y);
void initializePermutations();
#endif