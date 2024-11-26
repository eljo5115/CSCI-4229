#include "perlin.h"

int permutation[PERMUTATION_SIZE];

void initializePermutations(){
    for(int i = 0; i< PERMUTATION_SIZE;i++){
            permutation[i] = rand() % (PERMUTATION_SIZE);
    }
}

float perlinNoise(float x, float y) {
    // Determine grid cell coordinates
    int xi = (int)floor(x) & 255;
    int yi = (int)floor(y) & 255;

    // printf("xi: %d, yi: %d\n",xi,yi);
    // Relative x, y coordinates within cell
    float xf = x - floor(x);
    float yf = y - floor(y);
    // printf("xf: %f, yf: %f\n",xf,yf);

    // Fade curves
    float u = fade(xf);
    float v = fade(yf);

    // Hash coordinates of 4 cube corners
    int aa, ab, ba, bb;
    aa = permutation[permutation[xi] + yi];
    ab = permutation[permutation[xi] + yi + 1];
    ba = permutation[permutation[xi + 1] + yi];
    bb = permutation[permutation[xi + 1] + yi + 1];

    // Add blended results from the 4 corners
    float x1, x2;
    x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

    // printf("Lerp result of %f, %f and %f: %f\n",x,y,v, lerp(x1,x2,v));
    return lerp(x1, x2, v);
}

float fade(float t) {
    // Fade function as defined by Ken Perlin. This eases coordinate values
    // so that they will "ease" towards integral values. This ends up shaping
    // the noise beautifully.
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y) {
    switch(hash & 3) {
        case 0: return x + y;
        case 1: return -x + y;
        case 2: return x - y;
        case 3: return -x - y;
        default: return 0; // never happens
    }
}
