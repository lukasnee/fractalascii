#include "screen.h"

#include "config.h"

#include <ncurses.h>
#include <stdio.h>
#include "timer.h"

#define DISP_MAX_FPS CONFIG_MAX_FPS

Screen screen;

void ScreenReset()
{
	screen.pixelAspectRatio = 1.00;
	screen.showFPS = DISP_MAX_FPS;
	screen.exitRequest = 0;
}

int ScreenGetWidth()
{
	return getmaxx(stdscr) + 1;
}

int ScreenGetHeight()
{
	return getmaxy(stdscr) + 1;
}

double ScreenGetRatio()
{
	return (double)ScreenGetWidth()/(double)ScreenGetHeight();
}

// note. terminal ascii symbols usually have 2.0 aspect ratio
void ScreenSetPixelAspectRatio(double pixelAspectRatio)
{
	screen.pixelAspectRatio = pixelAspectRatio;
}

double ScreenGetPixelAspectRatio()
{
	return screen.pixelAspectRatio;
}

static void screenStart()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	clear();
	refresh();
}

static void screenShowFrame()
{
	if(screen.showFPS) 
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

void ScreenSetAsciiPixel(int x, int y, char asciiPixel)
{
	mvaddch(y, x, asciiPixel);
}

static void screenExit()
{
	screen.exitRequest = 1;
}

static void screenEnd()
{
	endwin();
}

void screenRun( void (*drawFnc)())
{
	screenStart();

	while(!screen.exitRequest)
	{
		drawFnc();
		screenShowFrame();
	}
	screenEnd();
}
