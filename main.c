
#include "screen.h"
#include "ncurses.h"
#include "mandelbrot.h"
#include "stats.h"
#include "config.h"

#include <stdint.h>

uint8_t showMandelbrotStats = CONFIG_SHOW_MANDELBROT_STATS;
const char intensityPallete[] = "   .-*^+=@#";
const uint16_t intensityPalleteSize = sizeof(intensityPallete)/sizeof(char) - 1;

static Stats intensity = { 0 };
Stats _intensity = { 0 };

static void drawCharByIntensity(uint16_t x, uint16_t y, uint16_t i)
{
    mvaddch(y, x, i <= intensityPalleteSize ? intensityPallete[i] : ' ');
}

void MandelbrotDrawPixel(uint16_t pxl_x, uint16_t pxl_y, double x, double y)
{
    int k = MandelbrotGet(x, y);
    drawCharByIntensity(pxl_x, pxl_y, 
        (uint16_t)(k + intensityPalleteSize/2 - intensity.avg));
    statsCollect(k, &_intensity);
}

static void actOnControls(GraphView * pV)
{
    double s = 0.5; // step in %
    double sDec = 1.00 - s;
    double sInc = 1.00 + s;
    double ms = 20; // move scale

   int c = getch();

    if(c == 'e')GraphViewSetScaleFactor(pV, sDec * pV->sf);
    if(c == 'q')GraphViewSetScaleFactor(pV, sInc * pV->sf); 
    if(c == 'w')GraphViewSetCenterY(pV, pV->cy + (pV->ymax - pV->ymin)/ms); 
    if(c == 'a')GraphViewSetCenterX(pV, pV->cx - (pV->xmax - pV->xmin)/ms); 
    if(c == 's')GraphViewSetCenterY(pV, pV->cy - (pV->ymax - pV->ymin)/ms); 
    if(c == 'd')GraphViewSetCenterX(pV, pV->cx + (pV->xmax - pV->xmin)/ms); 
    if(c == KEY_ENTER)
    {
        GraphViewSetCenterX(pV, 0.00); 
        GraphViewSetCenterY(pV, -1.00);    
    }
}

void MandelbrotStatsShow()
{
    char stats[100];
    statsToString(intensity, "intensity", stats);
    mvprintw(1, 0, stats); 
}

void MandelbrotStats()
{
    statsUpdate(&_intensity, &intensity);

    if(showMandelbrotStats) 
    {
        MandelbrotStatsShow();
    }
}

void MandelbrotDraw()
{
    static uint8_t init = 2;
    static GraphView view = { 0 }; 

    if(init)
    {
        GraphViewSetCenterX(&view, 0.0);
        GraphViewSetCenterY(&view, -1.0);
        GraphViewSetScaleFactor(&view, 4.0);
        init--;
    }
    else
    {
        actOnControls(&view);        
        //GraphViewSetScaleFactor(&view, view.sf*0.99);
    }
    



    double x, y; 
    int32_t pxl_x, pxl_y;
    for (pxl_y = 0; pxl_y < view.screen.height; pxl_y++)
    {
        y = view.ymax - pxl_y * view.dy;
        for (pxl_x = 0; pxl_x < view.screen.width; pxl_x++)
        {
            x = view.xmin + pxl_x * view.dx;

            MandelbrotDrawPixel(pxl_x, pxl_y, x, y);
        }
    } 
    
    int i = 2;
    mvprintw(i++, 0, "center (%f,%f)", view.cx, view.cy);
    mvprintw(i++, 0, "x-axis (%f,%f)", view.xmin, view.xmax);
    mvprintw(i++, 0, "y-axis (%f,%f)", view.ymin, view.ymax);        
    MandelbrotStats();
}

int main()
{   
    screenRun(MandelbrotDraw);
    return 0;
}

