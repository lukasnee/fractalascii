#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

typedef struct Screen_
{
    uint32_t width, height; // size in pixels
    double ratio; // derived on ScreenUpdate()
    double pixelAspectRatio;
}Screen;

extern const Screen screenDefault;

void ScreenUpdate();
void screenStart();
void ScreenEnd();
void ScreenFrameShow();
void screenExit();
void screenRun(void (*drawFnc)());

int ScreenGetWidth();
int ScreenGetHeight();
double ScreenGetRatio();
void ScreenSetPixelAspectRatio(double pixelAspectRatio);
double ScreenGetPixelAspectRatio();
#endif //SCREEN_H
