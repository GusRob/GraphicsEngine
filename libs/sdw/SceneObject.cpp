#include "SceneObject.h"

SceneObject::SceneObject(){}

SceneObject::SceneObject(std::vector<Triangle *> tris_init){
  triangles = tris_init;
}
