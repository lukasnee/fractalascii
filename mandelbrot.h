#pragma once

#include <stdint.h>

uint32_t MandelbrotGetMaxIterations();
void MandelbrotSetMaxIterations(uint32_t value);
uint32_t MandelbrotGet(double x, double y);
