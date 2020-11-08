#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

typedef struct Screen_
{
	double pixelAspectRatio;
	uint8_t showFPS;
	uint8_t exitRequest;
}Screen;

void ScreenReset();
int ScreenGetWidth();
int ScreenGetHeight();
double ScreenGetRatio();
void ScreenSetPixelAspectRatio(double pixelAspectRatio);
double ScreenGetPixelAspectRatio();

void ScreenSetAsciiPixel(int x, int y, char asciiPixel);
void screenRun(void (*drawFnc)());

#endif //SCREEN_H
