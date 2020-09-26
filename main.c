
#include "screen.h"
#include "ncurses.h"
#include "mandelbrot.h"
#include "stats.h"
#include "config.h"

#include <stdint.h>

uint8_t showMandelbrotStats = CONFIG_SHOW_MANDELBROT_STATS;
const char intensityPallete[] = " .-*^+=@#";
const uint16_t intensityPalleteSize = sizeof(intensityPallete)/sizeof(char) - 1;

static Stats intensity = { 0 };
Stats _intensity = { 0 };

static void drawCharByIntensity(uint16_t x, uint16_t y, uint16_t i)
{
    mvaddch(y, x, i <= intensityPalleteSize ? intensityPallete[i] : ' ');
}

void MandelbrotDrawPixel(double x, double y)
{
    int k = MandelbrotGet(x, y);
    drawCharByIntensity(x, y, 
        (uint16_t)(k/* + intensityPalleteSize/2intensity.avg*/));
    statsCollect(k, &_intensity);
}

static void actOnControls(GraphView * pView)
{
   int c = getch();
    switch (c)
    {
        case KEY_ENTER: screenExit(); break;
        case 'e': pView->sf -= 0.01 * pView->sf; break;
        case 'q': pView->sf += 0.01 * pView->sf; break;
        case 'w': pView->cy -= 0.01 * pView->sf; break;
        case 'a': pView->cx += 0.01 * pView->sf; break;
        case 's': pView->cy += 0.01 * pView->sf; break;
        case 'd': pView->cx -= 0.01 * pView->sf; break;
        default:break;
    }
}

void MandelbrotStatsShow()
{
    char stats[100];
    statsToString(intensity, "intensity", stats);
    mvprintw(getmaxy(stdscr), 0, stats); 
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
    static uint8_t init = 0;
    static GraphView view = { 0 }; 

    if(!init)
    {
        GraphViewSetCenterX(&view, 0);
        GraphViewSetCenterY(&view, 0);
        GraphViewSetScaleFactor(&view, 5);
        init = 1;
    }

    actOnControls(&view);

    double x, y; 
    int32_t i, j;
    for (j = 0; j < view.screen.height; j++)
    {
        y = view.ymax - j * view.dy;
        for (i = 0; i < view.screen.width; i++)
        {
            x = view.xmin + i * view.dx;

            MandelbrotDrawPixel(x, y);
        }
    }      
    MandelbrotStats();
}

int main()
{   
    screenRun(MandelbrotDraw);
    return 0;
}

