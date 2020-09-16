#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define DISP_HEIGHT 50
#define DISP_WIDTH 100
#define DISP_MAX_FPS 30
const char intensityPallete[] = " .:-=+*#%@";//"abcdefghijklmnopqrstuvxyz";
const uint16_t intensityPalleteSize = sizeof(intensityPallete)/sizeof(char) - 1;
static char dispBuffer[DISP_HEIGHT][DISP_WIDTH];

static uint32_t max_iterations;

static uint64_t dispAvgVal = 0;

static void displayDraw()
{   
    uint16_t x,y;
    dispAvgVal = 0;
    for(y = 0; y < DISP_HEIGHT; y++)
    {
        for(x = 0; x < DISP_WIDTH; x++)
        {
            dispAvgVal += dispBuffer[y][x];
            putchar(dispBuffer[y][x]);
        }
        printf("\r\n");
    }
    dispAvgVal /= DISP_HEIGHT * DISP_WIDTH;
    max_iterations = dispAvgVal;
}

static void displayPixelSet(uint16_t x, uint16_t y, char c)
{
    if(y < DISP_HEIGHT && x < DISP_WIDTH)
    {
        dispBuffer[y][x] = c;
    }
}

static void displayPixelClear(uint16_t x, uint16_t y)
{
    displayPixelSet(x, y, '.');
}

static void displayPixelSetByIntensity(uint16_t x, uint16_t y, uint16_t i)
{
    if(i <= intensityPalleteSize)
    {
        displayPixelSet(x, y, intensityPallete[i]);
    }
}

static void displayBufferClear()
{
    memset(dispBuffer, '.', DISP_HEIGHT*DISP_WIDTH);
}

void timePauseFor(uint32_t ms)
{
    uint64_t delta_ms = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while(delta_ms < ms)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delta_ms = (end.tv_sec - start.tv_sec) * 1000 + 
            (end.tv_nsec - start.tv_nsec) / 1000000;
    }
}

static void displayDrawFPS()
{
    static struct timespec lastFrameTimestamp, thisFrameTimestamp;

    clock_gettime(CLOCK_MONOTONIC_RAW, &thisFrameTimestamp);
    long fps = 1000000000/(thisFrameTimestamp.tv_nsec - lastFrameTimestamp.tv_nsec);
    sprintf(&dispBuffer[0][0], "[%ldFPS]", fps);
    lastFrameTimestamp = thisFrameTimestamp;
}

static void displayDoFrame()
{
    system("clear");
    displayDrawFPS();
    displayDraw();
    displayBufferClear();
   timePauseFor(1000/DISP_MAX_FPS);
}

static uint16_t mandelbrotGet(double x, double y)
{
    uint16_t iteration = 0;
    
    while (x*x + y*y <= 4.0 && iteration < max_iterations)
    {
        double xtemp = x*x - y*y;
        y = 2*x*y;
        x = xtemp;
        iteration++;
    }   
    return iteration;
}

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t mapDoubleToUint16(double x, double in_min, double in_max, uint16_t out_min, uint16_t out_max) 
{
  return (uint16_t)(
         (x - in_min) * (double)(out_max - out_min) / 
    (in_max - in_min) + (double)(out_min));
}

void mandelbrotDraw(double x, double y, double widthScale)
{
    double 
        x0 = x - widthScale/2, 
        x1 = x + widthScale/2,
        y0 = y - widthScale/2  * ((double)DISP_HEIGHT/DISP_WIDTH),
        y1 = y + widthScale/2  * ((double)DISP_HEIGHT/DISP_WIDTH);

    uint32_t pxIt = 0;

    double FxStep = (x1 - x0)/DISP_WIDTH;
    double FyStep = (y1 - y0)/DISP_HEIGHT;

    double Fx,Fy; // double iterators
    for(Fy = y0; Fy <= y1; Fy+=FyStep)
    {
        for(Fx = x0; Fx <= x1; Fx+=FxStep)
        {
            //system("clear");
            sprintf(&dispBuffer[1][0], "drawing pixel %d (%lf, %lf), avgVal=%lu\r\n", ++pxIt, Fx, Fy, dispAvgVal);

            uint16_t iteration = mandelbrotGet(Fx, Fy);
            //if(iteration)printf("iteration=%d\r\n", iteration);
            uint16_t intensity = map(iteration, 0, max_iterations, 0, intensityPalleteSize);
            uint16_t x = mapDoubleToUint16(Fx, x0, x1, 0, DISP_WIDTH-1);
            uint16_t y = mapDoubleToUint16(Fy, y0, y1, 0, DISP_HEIGHT-1);

            displayPixelSetByIntensity(x, y, intensity);    
        }
    }
}

#include <math.h>

void main()
{
    double s = 0.02;
    double 
        x = 0, 
        y = 1, 
        widthScale = 4;

    while(1)
    {
        mandelbrotDraw(x, y, widthScale);
        sprintf(&dispBuffer[2][0], 
            "center is (%lf, %lf), scale=%lf\r\n", x, y, widthScale);

        displayDoFrame();
        
        if(0)
        {
            switch((char)getchar())
            {
                case 'w': y -= 1 + y * s; break;
                case 'a': x -= 1 + x * s; break;
                case 's': y += 1 + y * s; break;
                case 'd': x += 1 + x * s; break;

                case 'e': widthScale -= widthScale * s; break;
                case 'q': widthScale += widthScale * s; break;
                
                default:break;
            }
        }
        else
        {
            widthScale -= widthScale * s; 
        }
    }
} 
