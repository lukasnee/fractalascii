#include <stdio.h>
#include <ncurses.h>
#include "stats.h"

uint16_t MandelbrotGet(double x, double y)
{
    static uint32_t max_iterations = 100;

    uint16_t k;
    double u = 0.0, v = 0.0;
    double u2 = u * u, v2 = v * v;

    for (k = 1; k < max_iterations && (u2 + v2 < 4.0); k++)
    {
        v = 2 * u * v + y;
        u = u2 - v2 + x;
        u2 = u * u;
        v2 = v * v;
    };

    return k;
}
