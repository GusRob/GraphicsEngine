#pragma once

#include <iostream>
#include "Vector.h"
#include "SceneObject.h"
#include <vector>
#include "Matrix.h"
#include "DrawingWindow.h"

struct Scene {
    Vector windowDim;
    Vector camera = Vector(0, 0, -10);
    Matrix cameraAngle = Matrix(1);
	std::vector<SceneObject> objects;
    
    std::vector<std::vector<float>> depthBuf;
    
	Scene(int canvasWidth, int canvasHeight);
    void addObject(SceneObject obj);
    void resetBuf();
    void lookAt(Vector target);
    
    
    void rasterScene(DrawingWindow &window);
};
