#include "fractalascii.h"

#include "screen.h"
#include "camera.h"
#include "stats.h"

#include "mandelbrot.h"
#include "config.h"

#include "ncurses.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define AVERAGE(a, b) (((a) + (b)) / 2)
#define MAP(x, in_min, in_max, out_min, out_max) \
(((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

static void FractalasciiDraw(Fractalascii * pFascii);

static void FractalasciiReset(Fractalascii * pFascii)
{
	ScreenReset();
	CameraReset(&pFascii->cam);
	StatsReset(&pFascii->iterationStats);
	pFascii->showStats = CONFIG_SHOW_STATS_BY_DEFAULT;
	pFascii->showHelpWindow = CONFIG_SHOW_HELP_WINDOW_BY_DEFAULT;
}

static void FractalasciiInit(Fractalascii * pFascii, 
	const char * asciiPallete, double pixelAspectRatio)
{
	pFascii->asciiPallete = (char *)asciiPallete;
	pFascii->asciiPalleteSize = strlen(asciiPallete);

	ScreenSetPixelAspectRatio(pixelAspectRatio);
	CameraSetPosition(&pFascii->cam, -0.748158,0.088871, 1.0); //default

	uint8_t init = 1; 
	while(init--)
	{
		FractalasciiDraw(pFascii);
	}
}

// makes sure zoom and deltas are in synch if screen dimensions changes
static void FractalasciiAdjustToScreen(Fractalascii * pFascii)
{
	static int prevWidth = 1; 
	static int prevHeight = 1;

	double scaleDelta = AVERAGE((double)ScreenGetWidth() / (double)prevWidth, 
								(double)ScreenGetHeight() / (double)prevHeight);
			
	CameraZoom(&pFascii->cam, scaleDelta);

	pFascii->dx = CameraGetWidth(&pFascii->cam) / 
		ScreenGetWidth(&pFascii->cam);

 	pFascii->dy = CameraGetHeight(&pFascii->cam) / 
		ScreenGetHeight(&pFascii->cam) / 
	 		ScreenGetPixelAspectRatio();

	CameraSetCenterX(&pFascii->cam, CameraGetCenterX(&pFascii->cam) + 
		(double)(ScreenGetWidth() - prevWidth) * pFascii->dx / 2.0);

	CameraSetCenterY(&pFascii->cam, CameraGetCenterY(&pFascii->cam) + 
		(double)(ScreenGetHeight() - prevHeight) * pFascii->dy / 2.0);

	prevWidth = ScreenGetWidth(); 
	prevHeight = ScreenGetHeight();
}

static int32_t FractalasciiMapIterationToAsciiPallete(Fractalascii * pFascii, 
	uint32_t k)
{
	return (uint32_t)MAP(k, 
		(int32_t)StatsGetMin(pFascii->iterationStats), 
		(int32_t)StatsGetMax(pFascii->iterationStats), 
		(int32_t)0, 
		(int32_t)pFascii->asciiPalleteSize
	);

}

static uint32_t FractalasciiNormalize(Fractalascii * pFascii, uint32_t k)
{
	if(k > 2 && k < MandelbrotGetMaxIterations() &&
		(uint32_t)StatsGetMin(pFascii->iterationStats) < 
			(uint32_t)StatsGetMax(pFascii->iterationStats))
	{
		k = FractalasciiMapIterationToAsciiPallete(pFascii, k);
	}
	return k;
}

static void FractalasciiDrawCharByIntensity(Fractalascii * pFascii, 
	uint16_t x, uint16_t y, uint32_t i)
{
	ScreenSetAsciiPixel(x, y, 
		i <= pFascii->asciiPalleteSize ? pFascii->asciiPallete[i] : ' ');
}

uint32_t FractalasciiDrawPixel(Fractalascii * pFascii, 
	uint16_t pxl_x, uint16_t pxl_y, 
	double x, double y)
{
	uint32_t k, kn;
	k = MandelbrotGet(x, y);
	kn = (uint32_t)FractalasciiNormalize(pFascii, k);
	FractalasciiDrawCharByIntensity(pFascii, pxl_x, pxl_y, kn);
	return k;
}

static void FractalasciiDraw(Fractalascii * pFascii)
{
	Stats iterationStats;
	double x, y;  
	int32_t pxl_x, pxl_y;
	for (pxl_y = 0; pxl_y < ScreenGetHeight(); pxl_y++)
	{
		y = CameraGetYmax(&pFascii->cam) - pxl_y * pFascii->dy;
		for (pxl_x = 0; pxl_x < ScreenGetWidth(); pxl_x++)
		{
			x = CameraGetXmin(&pFascii->cam) + pxl_x * pFascii->dx;
			uint32_t k = FractalasciiDrawPixel(pFascii, pxl_x, pxl_y, x, y);
			StatsCollect(k, &iterationStats);
		}
	} 
	StatsRecord(&iterationStats, &pFascii->iterationStats);
}

static void FractalasciiDrawStats(Fractalascii * pFascii, int16_t x, int16_t y)
{
	if(pFascii->showStats) 
	{
		char statsStr[0x200];
		StatsToString(pFascii->iterationStats, "iteration", statsStr);

		mvprintw(y++, x, statsStr); 
		mvprintw(y++, x, "center\t[x:%e, y:%e]", 
			CameraGetCenterX(&pFascii->cam), CameraGetCenterY(&pFascii->cam)); 
		mvprintw(y++, x, "width\t[%e]", CameraGetWidth(&pFascii->cam)); 
		mvprintw(y++, x, "height\t[%e]", CameraGetHeight(&pFascii->cam)); 
	}
}

static void FractalasciiHelpWindow(Fractalascii * pFascii)
{
	if(pFascii->showHelpWindow)
	{
		WINDOW * ctrlWin;
		static int w = 40, h = 12;
		int y = 0, x = 0;

		ctrlWin = derwin(stdscr, h, w, 
			ScreenGetHeight() / 2 - h, 
			ScreenGetWidth() / 2 - w);
		
		wclear(ctrlWin);
		box(ctrlWin, 0, 0); y++; x++;
        
		mvwprintw(ctrlWin, y++, x, "\t\tControls");
		mvwhline(ctrlWin, y++, x, '-',  w - 2 * x);
		mvwprintw(ctrlWin, y++, x, "`h` - see this help window");
		mvwprintw(ctrlWin, y++, x, "`w`, `a`, `s`, `d` - move");
		mvwprintw(ctrlWin, y++, x, "`e` - zoom in 10%");
		mvwprintw(ctrlWin, y++, x, "`q` - zoom out 10%");
		mvwprintw(ctrlWin, y++, x, "`r` - reset camera");
		mvwprintw(ctrlWin, y++, x, "`g` - see stats");
		mvwprintw(ctrlWin, y++, x, "`c` - exit");
		mvwprintw(ctrlWin, y++, x, "`1`, `2`, `3` ... - interesting views");
        y++; h = y;

		wrefresh(ctrlWin);
	}
}

static void FractalasciiReactToControls(Fractalascii * pFascii)
{
	double s = 0.5; // step in %
	double sDec = 1.00 - s;
	double sInc = 1.00 + s;
	double ms = 20; // move scale

   int c = getch();

	if(c == 'h') 
	{
		pFascii->showHelpWindow = !pFascii->showHelpWindow;
	}
	if(c == 'g') 
	{
		pFascii->showStats = !pFascii->showStats;
	}
	if(c == 'e')
	{
		CameraZoom(&pFascii->cam, sDec);
	}
	if(c == 'q')
	{
		CameraZoom(&pFascii->cam, sInc); 
	}
	if(c == 'w')
	{
		CameraSetCenterY(&pFascii->cam, 
			CameraGetCenterY(&pFascii->cam) + 
				(CameraGetHeight(&pFascii->cam)) / ms); 
	}
	if(c == 'a')
	{
		CameraSetCenterX(&pFascii->cam, 
			CameraGetCenterX(&pFascii->cam) - 
				(CameraGetWidth(&pFascii->cam)) / ms); 
	}
	if(c == 's')
	{
		CameraSetCenterY(&pFascii->cam, 
			CameraGetCenterY(&pFascii->cam) - 
				(CameraGetHeight(&pFascii->cam)) / ms);
	} 
	if(c == 'd')
	{
		CameraSetCenterX(&pFascii->cam, 
			CameraGetCenterX(&pFascii->cam) + 
				(CameraGetWidth(&pFascii->cam)) / ms); 
	}
	if(c == 'r')
	{
		CameraSetPosition(&pFascii->cam, 0.0, 0.0, 0.0);
	}
	
	if(c == 'c')
	{
		ScreenExit();
	}

	if(c == '1')
	{
		CameraSetPosition(&pFascii->cam, -0.761574, -0.08475, 6.0);
	}
	if(c == '2')
	{
		CameraSetPosition(&pFascii->cam, 
			-7.362165077819709e-01, -2.071562798303199e-01, 6.0);
	}
}

static void FractalasciiRun(void * p)
{
	Fractalascii * pFascii = (Fractalascii *)p;
	//FractalasciiAdjustToScreen(pFascii);
	FractalasciiDraw(pFascii);
	FractalasciiDrawStats(pFascii, 0, 1);
	FractalasciiHelpWindow(pFascii);
	
	ScreenRefresh();

	FractalasciiReactToControls(pFascii); 
}

void FractalasciiStart(const char * asciiPallete, double pixelAspectRatio)
{	
	Fractalascii fa;
	FractalasciiReset(&fa);
	FractalasciiInit(&fa, asciiPallete, pixelAspectRatio);
	ScreenRun(FractalasciiRun, (void *)&fa);
}
