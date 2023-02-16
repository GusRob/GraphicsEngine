#pragma once

#include <DrawingWindow.h>
#include <Vector.h>
#include <Triangle.h>
#include <Colour.h>

uint32_t packCol(Colour col);
Colour unpackCol(uint32_t col);
Vector unpackColToVec(uint32_t col);
Colour dimCol(Colour col, float dimness);
Colour brightCol(Colour col, float brightPerc);

std::vector<float> linearInterpolation(float start, float end, int count);

void drawOval(DrawingWindow &window, Vector p, int r, Colour col);

void drawLine(DrawingWindow &window, Vector p0, Vector p1, Colour col);

void drawTriangle(DrawingWindow &window, Triangle tri);

void fillTriangle(DrawingWindow &window, Triangle tri);
