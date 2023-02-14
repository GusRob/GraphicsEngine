#pragma once

#include <DrawingWindow.h>
#include <Vector.h>
#include <Scene.h>
#include <tuple>

std::tuple<bool, Vector, Triangle *> getClosestPointOnRay(Scene &scene, Vector ray, Vector rayOrigin);

Colour interpTexture(Vector p, Triangle *tri);

bool canSeePoint(Scene &scene, Vector origin, Vector target);

uint32_t getColourAtPoint(Scene &scene, Vector p, Triangle *tri, Vector fromDir);

void drawRayTrace(DrawingWindow &window, Scene &scene, Vector pixel);
