
#include "camera.h"

#include <stdio.h>
#include "screen.h"

const GraphView graphViewDefault = {
	.sf = 5,
	.cx = 0.0,
	.cy = 0.0,
    .xmin = -0.5,
    .xmax = 0.5,
    .ymin = -0.5,
    .ymax = 0.5,
    .dx = 0.01,
    .dy = 0.01
};

void GraphViewReset(GraphView * pView)
{
	*pView = graphViewDefault;
}

void GraphViewUpdateMinMax(GraphView * pView)
{
	pView->xmin = pView->cx - pView->sf / 2.0F;
	pView->xmax = pView->cx + pView->sf / 2.0F;
	pView->ymin = pView->cy - pView->sf / 2.0F * 
        ScreenGetPixelAspectRatio();
	pView->ymax = pView->cy + pView->sf / 2.0F * 
        ScreenGetPixelAspectRatio();
}

void GraphViewUpdateDeltas(GraphView * pView)
{
	pView->dx = (pView->xmax - pView->xmin) / 
        (double)ScreenGetWidth();
	pView->dy = (pView->ymax - pView->ymin) / 
        (double)ScreenGetHeight();
}

void GraphViewUpdatePerspective(GraphView * pView)
{
	GraphViewUpdateMinMax(pView);
	GraphViewUpdateDeltas(pView);
}

void GraphViewUpdate(GraphView * pView)
{
	GraphViewUpdatePerspective(pView);
}

double GraphViewGetCenterX(GraphView * pView)
{
	return pView->cx;
}

void GraphViewSetCenterX(GraphView * pView, double cx)
{
	pView->cx = cx;
	GraphViewUpdate(pView);
}

double GraphViewGetCenterY(GraphView * pView)
{
	return pView->cy;
}

void GraphViewSetCenterY(GraphView * pView, double cy)
{
	pView->cy = cy;
	GraphViewUpdate(pView);
}

double GraphViewGetScaleFactor(GraphView * pView)
{
	return pView->sf;
}

void GraphViewSetScaleFactor(GraphView * pView, double sf)
{
	pView->sf = sf;
	GraphViewUpdate(pView);
}

int GraphViewGetCenterInString(GraphView * pView, char * pString)
{
	return sprintf(pString, "center (%f,%f) ", pView->cx, pView->cy);
}

int GraphViewGetXaxisInString(GraphView * pView, char * pString)
{
	return sprintf(pString, "x-axis (%f,%f) ", pView->xmin, pView->xmax);
}

int GraphViewGetYaxisInString(GraphView * pView, char * pString)
{
	return sprintf(pString, "y-axis (%f,%f) ", pView->ymin, pView->ymax);
}

int GraphViewGetStatsInString(GraphView * pView, char * pString)
{
	char * pS = pString;
	pS += GraphViewGetCenterInString(pView, pS);
    pS += GraphViewGetXaxisInString(pView, pS);
    pS += GraphViewGetYaxisInString(pView, pS);
    return (int)(pS - pString);
}
