#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

typedef struct Screen_
{
    uint32_t width, height; // size in pixels

     // derived (on update)
    double ratio;
}Screen;

//todo add rotation
typedef struct GraphView_
{
    Screen screen;
    double 
        sf,        // scale factor
        cx, cy;    // center

    // derived (on update)
    double 
        xmin, xmax,
        ymin, ymax,
        dx,
        dy;
}GraphView;

void screenStart();
void ScreenEnd();
void ScreenFrameShow();
void screenExit();
void screenRun(void (*drawFnc)());

extern const Screen screenDefault;
extern const GraphView graphViewDefault;

void GraphViewReset(GraphView * pView);
void GraphViewScreenUpdate(Screen * pScreen);
void GraphViewUpdateMinMax(GraphView * pView);
void GraphViewUpdateDeltas(GraphView * pView);
void GraphViewUpdatePerspective(GraphView * pView);
void GraphViewUpdate(GraphView * pView);
void GraphViewSetCenterX(GraphView * pView, double cx);
void GraphViewSetCenterY(GraphView * pView, double cy);
void GraphViewSetScaleFactor(GraphView * pView, double sf);

#endif //SCREEN_H
