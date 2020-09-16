#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define DISP_HEIGHT 80
#define DISP_WIDTH 150
#define DISP_MAX_FPS 30
const char intensityPallete[] = " .:-=+*#%@";//"abcdefghijklmnopqrstuvxyz";
const uint16_t intensityPalleteSize = sizeof(intensityPallete)/sizeof(char) - 1;
static char dispBuffer[DISP_HEIGHT][DISP_WIDTH];

static uint32_t max_iterations = intensityPalleteSize;

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

void mandelbrotDraw(double cx, double cy, double widthScale)
{
    uint32_t pxIt = 0;
    double 
        xmin = cx - widthScale/2, 
        xmax = cx + widthScale/2,
        ymin = cy - widthScale/2  * ((double)DISP_HEIGHT/DISP_WIDTH),
        ymax = cy + widthScale/2  * ((double)DISP_HEIGHT/DISP_WIDTH),
        xres = DISP_WIDTH,
        yres = DISP_HEIGHT;

    double dx = (xmax - xmin) / xres;
    double dy = (ymax - ymin) / yres;

    double x, y; /* Coordinates of the current point in the complex plane. */
    double u, v; /* Coordinates of the iterated point. */
    int i, j;    /* Pixel counters */
    int k;       /* Iteration counter */
    for (j = 0; j < yres; j++)
    {
        y = ymax - j * dy;
        for (i = 0; i < xres; i++)
        {
            double u = 0.0;
            double v = 0.0;
            double u2 = u * u;
            double v2 = v * v;
            x = xmin + i * dx;
            /* iterate the point */
            for (k = 1; k < max_iterations && (u2 + v2 < 4.0); k++)
            {
                v = 2 * u * v + y;
                u = u2 - v2 + x;
                u2 = u * u;
                v2 = v * v;
            };
            /* compute  pixel color and write it to file */
            if (k >= max_iterations)
            {
                displayPixelSetByIntensity(x, y, intensityPalleteSize-1);
            }
            else
            {
                uint16_t intensity = map(k, 0, max_iterations, 0, intensityPalleteSize);
                displayPixelSetByIntensity(x, y, intensity);
                sprintf(&dispBuffer[1][0], "drawing pixel %d (%lf, %lf), avgVal=%lu\r\n", ++pxIt, u, v, dispAvgVal);
            };
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
