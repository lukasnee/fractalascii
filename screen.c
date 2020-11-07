#include "screen.h"

#include <ncurses.h>
#include <stdio.h>
#include "timer.h"

#include "config.h"

#define DISP_MAX_FPS CONFIG_MAX_FPS

uint8_t showFPS = CONFIG_SHOW_FPS;
uint8_t exitScreenFlag = 0;

const Screen screenDefault = {
	.width = 0,
	.height = 0,
	.ratio = 0.0,
    .pixelAspectRatio = 1.0
};

Screen screen = screenDefault;

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

void screenRun( void (*drawFnc)())
{
	screenStart();

	while(!exitScreenFlag)
	{
		drawFnc();
		screenShowFrame();
	}
	screenEnd();
}

void ScreenUpdate()
{
	screen.width = ScreenGetWidth();
	screen.height = ScreenGetHeight();
	screen.ratio = ScreenGetRatio();
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

void ScreenSetPixelAspectRatio(double pixelAspectRatio)
{
    screen.pixelAspectRatio = pixelAspectRatio;
}
double ScreenGetPixelAspectRatio()
{
    return screen.pixelAspectRatio;
}