#include "SceneObject.h"

SceneObject::SceneObject(){
}

SceneObject::SceneObject(std::vector<Triangle *> tris_init){
  triangles = tris_init;
}

void SceneObject::calcCollisionSphere(){
  int vertexCount = 0;
  Vector sum = Vector(0, 0, 0);
  for(Triangle *tri : triangles){
    vertexCount+=3;
    sum = sum + tri->p0 + tri->p1 + tri->p2;
  }
  centerMass = sum / vertexCount;
  collisionSphereRadius = 0;
  for(Triangle *tri : triangles){
    float r = std::max(std::max(size(tri->p0 - centerMass), size(tri->p1 - centerMass)), size(tri->p2 - centerMass));
    if(r > collisionSphereRadius){
      collisionSphereRadius = r;
    }
  }

}
