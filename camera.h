#ifndef CAMERA_H
#define CAMERA_H

//todo add rotation
typedef struct GraphView_
{
    double 
        sf,        // scale factor
        cx, cy;    // center

    // derived on GraphViewUpdate():
    double 
        xmin, xmax,
        ymin, ymax,
        dx,
        dy;
}GraphView;

extern const GraphView graphViewDefault;

void GraphViewReset(GraphView * pView);
void GraphViewUpdateMinMax(GraphView * pView);
void GraphViewUpdateDeltas(GraphView * pView);
void GraphViewUpdatePerspective(GraphView * pView);
void GraphViewUpdate(GraphView * pView);
double GraphViewGetCenterY(GraphView * pView);
void GraphViewSetCenterX(GraphView * pView, double cx);
double GraphViewGetCenterY(GraphView * pView);
void GraphViewSetCenterY(GraphView * pView, double cy);
double GraphViewGetScaleFactor(GraphView * pView);
void GraphViewSetScaleFactor(GraphView * pView, double sf);
int GraphViewGetCenterInString(GraphView * pView, char * pString);
int GraphViewGetXaxisInString(GraphView * pView, char * pString);
int GraphViewGetYaxisInString(GraphView * pView, char * pString);
int GraphViewGetStatsInString(GraphView * pView, char * pString);

#endif //CAMERA_H