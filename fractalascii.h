#ifndef FRACTALASCII_H
#define FRACTALASCII_H

#include <stdint.h>

#include "screen.h"
#include "camera.h"
#include "stats.h"

typedef struct Fractalascii_
{
	Camera cam;
    double dx, dy; // camera plain per screen pixel
	char * asciiPallete; 
    uint16_t asciiPalleteSize;
	Stats iterationStats;
	uint8_t showStats;

} Fractalascii;

void FractalasciiStart(const char * asciiPallete, double pixelAspectRatio);

#endif //FRACTALASCII_H