#pragma once

#include <DrawingWindow.h>
#include <Vector.h>
#include <Scene.h>

Vector getPointOnCanvas(Scene &scene, Vector p);

void draw3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri);

void fill3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri);
