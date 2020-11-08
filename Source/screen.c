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

static void ScreenStart()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	clear();
	refresh();
}

static void ScreenProcessFPSOverlay()
{
	if(screen.showFPS) 
	{
		static Timer fpstimer;
		uint32_t fps;
		timerGetFPS(&fpstimer, &fps);
		mvprintw(0, 0, "[%ldFPS]", fps);
	}
}

static void ScreenShowFrame()
{
	ScreenProcessFPSOverlay();
}

void ScreenSetAsciiPixel(int x, int y, char asciiPixel)
{
	mvaddch(y, x, asciiPixel);
}

static void ScreenLimitFPS()
{
	if(DISP_MAX_FPS)
	{
		timerDelayMs(1000/DISP_MAX_FPS); //todo legit, non-blocking 
	}
}

void ScreenExit()
{
	screen.exitRequest = 1;
}

static void ScreenEnd()
{
	endwin();
}

void ScreenRefresh()
{
	refresh();
}

void ScreenRun(void (*drawFnc)(void * p), void * p)
{
	ScreenStart();

	while(!screen.exitRequest)
	{
		drawFnc(p);
		ScreenShowFrame();
		ScreenLimitFPS();
	}
	ScreenEnd();
}
