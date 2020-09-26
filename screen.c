#include "screen.h"

#include <ncurses.h>
#include <stdio.h>
#include "timer.h"

#include "config.h"

#define DISP_MAX_FPS CONFIG_MAX_FPS

uint8_t showFPS = CONFIG_SHOW_FPS;
uint8_t exitScreenFlag = 0;

void screenStart()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    refresh();
}

void screenEnd()
{
    endwin();
}

void screenShowFrame()
{
    if(showFPS) 
    {
        static Timer fpstimer;
        uint32_t fps;
        timerGetFPS(&fpstimer, &fps);
        mvprintw(0, 0, "[%ldFPS]", fps);
    }
    refresh();
    if(DISP_MAX_FPS)
    {
        timerDelayMs(1000/DISP_MAX_FPS);
    }
}

void screenExit()
{
    exitScreenFlag = 1;
}

void screenRun(void (*drawFnc)())
{
    screenStart();

    while(!exitScreenFlag)
    {
        drawFnc();
        screenShowFrame();
    }
    screenEnd();
}

const Screen screenDefault = {
    .width = 0,
    .height = 0,
    .ratio = 0.0F
};

const GraphView graphViewDefault = {
    .screen = screenDefault,
    .sf = 5,
    .cx = 0.0F,
    .cy = 0.0F
};

void GraphViewReset(GraphView * pView)
{
    *pView = graphViewDefault;
}

void GraphViewScreenUpdate(Screen * pScreen)
{
    pScreen->width = getmaxx(stdscr) + 1;
    pScreen->height = getmaxy(stdscr) + 1;
    pScreen->ratio = (double)pScreen->width/(double)pScreen->height;
}

void GraphViewUpdateMinMax(GraphView * pView)
{
    pView->xmin = pView->cx - pView->sf / 2.0F;
    pView->xmax = pView->cx + pView->sf / 2.0F;
    pView->ymin = pView->cy - pView->sf / 2.0F * pView->screen.ratio;
    pView->ymax = pView->cy + pView->sf / 2.0F * pView->screen.ratio;
}

void GraphViewUpdateDeltas(GraphView * pView)
{
    pView->dx = (pView->xmax - pView->xmin) / (double)pView->screen.width;
    pView->dy = (pView->ymax - pView->ymin) / (double)pView->screen.height;
}

void GraphViewUpdatePerspective(GraphView * pView)
{
    GraphViewUpdateMinMax(pView);
    GraphViewUpdateDeltas(pView);
}

void GraphViewUpdate(GraphView * pView)
{
    GraphViewScreenUpdate(&pView->screen);
    GraphViewUpdatePerspective(pView);
}

void GraphViewSetCenterX(GraphView * pView, double cx)
{
    pView->cx = cx;
    GraphViewUpdate(pView);
}

void GraphViewSetCenterY(GraphView * pView, double cy)
{
    pView->cy = cy;
    GraphViewUpdate(pView);
}

void GraphViewSetScaleFactor(GraphView * pView, double sf)
{
    pView->sf = sf;
    GraphViewUpdate(pView);
}
