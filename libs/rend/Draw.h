#pragma once

#include <DrawingWindow.h>
#include <Vector.h>

std::vector<float> linearInterpolation(float start, float end, int count);

void drawOval(DrawingWindow &window, Vector p, int r, uint32_t col);

void drawLine(DrawingWindow &window, Vector p1, Vector p2, uint32_t col);

void drawTriangle(DrawingWindow &window, Vector p1, Vector p2, Vector p3, uint32_t col);

void fillTriangle(DrawingWindow &window, Vector p1, Vector p2, Vector p3, uint32_t col);
