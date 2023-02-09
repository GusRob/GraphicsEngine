#include "Scene.h"
#include <iostream>

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