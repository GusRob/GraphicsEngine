#pragma once

#include <DrawingWindow.h>
#include <Vector.h>
#include <Scene.h>
#include <tuple>

std::tuple<bool, Vector, Triangle *> getClosestPointOnRay(Scene &scene, Vector ray, Vector rayOrigin);

uint32_t getColourOfTriAtPoint(Vector p, Triangle *tri);

void drawRayTrace(DrawingWindow &window, Scene &scene, Vector pixel);
