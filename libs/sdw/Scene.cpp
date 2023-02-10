#include "Scene.h"
#include <iostream>
#include <Raster.h>

Scene::Scene(int canvasWidth, int canvasHeight){
    windowDim = Vector(canvasWidth, canvasHeight);
    for(int i = 0; i < windowDim.x; i++){
        depthBuf.push_back(std::vector<float>(windowDim.y));
        fill(depthBuf[i].begin(), depthBuf[i].end(), -999999);
    }
}

void Scene::addObject(SceneObject obj){
    objects.push_back(obj);
}

void Scene::resetBuf(){
    for(int i = 0; i < windowDim.x; i++){
        fill(depthBuf[i].begin(), depthBuf[i].end(), -999999);
    }
}

void Scene::lookAt(Vector target){
    Vector forward = norm(camera - target);
	Vector right = norm(cross(Vector(0, 1, 0), forward));
	Vector up = norm(cross(forward, right));

	cameraAngle = Matrix(right, up, forward);

}


void Scene::rasterScene(DrawingWindow &window){
    this->resetBuf();
    for(SceneObject obj : objects){
        for(Triangle tri : obj.triangles){
            fill3DTriangle(window, *this, tri);
        }
    }
}