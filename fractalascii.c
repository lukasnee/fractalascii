#include "fractalascii.h"

#include "screen.h"
#include "camera.h"
#include "stats.h"

#include "mandelbrot.h"
#include "config.h"

#include "ncurses.h"

#include <string.h>
#include <stdint.h>

#define AVERAGE(a, b) (((a) + (b)) / 2)
#define MAP(x, in_min, in_max, out_min, out_max) \
(((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

Fractalascii fa;

static int64_t FractalasciiNormalize(int64_t k)
{
	if(k > 2 && k < MandelbrotGetMaxIterations() &&
		statsGetMin(fa.iterationStats) < statsGetMax(fa.iterationStats))
	{
		k = MAP(k, 
				statsGetMin(fa.iterationStats), 
				statsGetMax(fa.iterationStats), 
				(int64_t)0, 
				(int64_t)fa.asciiPalleteSize);
	}
	return k;
}

static void FractalasciiDrawCharByIntensity(uint16_t x, uint16_t y, uint16_t i)
{
	ScreenSetAsciiPixel(x, y, i <= fa.asciiPalleteSize ? fa.asciiPallete[i] : ' ');
}

uint32_t FractalasciiDrawPixel(uint16_t pxl_x, uint16_t pxl_y, 
	double x, double y)
{
	uint32_t k, kn;
	k = MandelbrotGet(x, y);
	kn = (uint16_t)FractalasciiNormalize(k);
	FractalasciiDrawCharByIntensity(pxl_x, pxl_y, kn);
	return k;
}

// makes sure zoom and deltas are in synch if screen dimensions changes
static void FractalasciiAdjustToScreen()
{
	static double prevWidth, prevHeight;
	double scaleDelta;
	scaleDelta = AVERAGE(ScreenGetWidth() / prevWidth, 
						ScreenGetHeight() / prevHeight);
		
	CameraZoom(&fa.cam, scaleDelta);
	
	prevWidth = (int)ScreenGetWidth(); 
	prevHeight = (int)ScreenGetHeight();

	fa.dx = CameraGetWidth(&fa.cam) / ScreenGetWidth(&fa.cam);
 	fa.dy = CameraGetHeight(&fa.cam) / ScreenGetHeight(&fa.cam) * 
	 	ScreenGetPixelAspectRatio();
}

static void FractalasciiDraw()
{
	Stats iterationStats;
	double x, y;  
	int32_t pxl_x, pxl_y;
	for (pxl_y = 0; pxl_y < ScreenGetHeight(); pxl_y++)
	{
		y = CameraGetYmax(&fa.cam) - pxl_y * fa.dy;
		for (pxl_x = 0; pxl_x < ScreenGetWidth(); pxl_x++)
		{
			x = CameraGetXmin(&fa.cam) + pxl_x * fa.dx;
			uint32_t k = FractalasciiDrawPixel(pxl_x, pxl_y, x, y);
			statsCollect(k, &iterationStats);
		}
	} 

	statsRecord(&iterationStats, &fa.iterationStats);
}

static void FractalasciiStatsShow(int16_t x,int16_t y)
{
	char statsStr[0x200];
	statsToString(fa.iterationStats, "iteration", statsStr);
	mvprintw(y++, x, statsStr); 
	mvprintw(y++, x, "center\t[x:%e, y:%e]", CameraGetCenterX(&fa.cam), CameraGetCenterY(&fa.cam)); 
	mvprintw(y++, x, "width\t[%e]", CameraGetWidth(&fa.cam)); 
	mvprintw(y++, x, "height\t[%e]", CameraGetHeight(&fa.cam)); 
}

static void FractalasciiStats(int16_t x,int16_t y)
{
	if(fa.showStats) 
	{
		FractalasciiStatsShow(x, y);
	}
}

static void FractalasciiReactToControls(Fractalascii * pFA)
{
	double s = 0.5; // step in %
	double sDec = 1.00 - s;
	double sInc = 1.00 + s;
	double ms = 20; // move scale

   int c = getch();

	if(c == 'e')CameraZoom(&pFA->cam, sDec);
	if(c == 'q')CameraZoom(&pFA->cam, sInc); 
	if(c == 'w')CameraSetCenterY(&pFA->cam, CameraGetCenterY(&pFA->cam) + (CameraGetHeight(&pFA->cam))/ms); 
	if(c == 'a')CameraSetCenterX(&pFA->cam, CameraGetCenterX(&pFA->cam) - (CameraGetWidth(&pFA->cam))/ms); 
	if(c == 's')CameraSetCenterY(&pFA->cam, CameraGetCenterY(&pFA->cam) - (CameraGetHeight(&pFA->cam))/ms); 
	if(c == 'd')CameraSetCenterX(&pFA->cam, CameraGetCenterX(&pFA->cam) + (CameraGetWidth(&pFA->cam))/ms); 

	if(c == 'r')CameraSetPosition(&pFA->cam, 0.0,       0.0,        0.0);
	if(c == '1')CameraSetPosition(&pFA->cam, -0.761574, -0.08475,   6.0);
	if(c == '2')CameraSetPosition(&pFA->cam, -7.362165077819709e-01, -2.071562798303199e-01,   6.0);
	
	if(c == 'h')
	{
		WINDOW * win;
		win = newwin(30, 30, ScreenGetHeight()/2-30/2, ScreenGetHeight()/2-30/2);
		wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
		mvwprintw(win, 0, 0, 
			" # Controls\n\r\
			- `w`, `a`, `s`, `d` - move\n\r\
			- `e` - zoom in 10%\n\r\
			- `q` - zoom out 10%\n\r\
			- `r` - reset camera\n\r\
			- `1`, `2`, `3` ... - interesting views\n\r");
		wrefresh(win);
	}
}

static void FractalasciiRun()
{
	FractalasciiAdjustToScreen();
	FractalasciiDraw();
	FractalasciiStats(0, 0);
	FractalasciiReactToControls(&fa); 
}

void FractalasciiStart(const char * asciiPallete, double pixelAspectRatio)
{	
	ScreenReset();
	ScreenSetPixelAspectRatio(pixelAspectRatio);
	CameraReset(&fa.cam);
	fa.asciiPallete = (char *)asciiPallete;
	fa.asciiPalleteSize = strlen(asciiPallete);
	statsReset(&fa.iterationStats);
	fa.showStats = CONFIG_SHOW_MANDELBROT_STATS;

	CameraSetPosition(&fa.cam, -0.748158,0.088871, 6.0);
	
	// iterate twice to better process initial image 
	uint8_t init = 2; 
	while(init--)
	{
		FractalasciiDraw();
	}

	screenRun(FractalasciiRun);
}