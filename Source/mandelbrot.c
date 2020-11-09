#include <stdio.h>
#include <ncurses.h>
#include "stats.h"

static uint32_t max_iterations = 5000;

uint32_t MandelbrotGetMaxIterations()
{
	return max_iterations;
}

void MandelbrotSetMaxIterations(uint32_t value)
{
	max_iterations = value;
}

uint32_t MandelbrotGet(double x, double y)
{

	uint32_t k;
	double u = 0.0, v = 0.0;
	double u2 = u * u, v2 = v * v;

	for (k = 1; k < MandelbrotGetMaxIterations() && (u2 + v2 < 4.0); k++)
	{
		v = 2 * u * v + y;
		u = u2 - v2 + x;
		u2 = u * u;
		v2 = v * v;
	};

	return k;
}
