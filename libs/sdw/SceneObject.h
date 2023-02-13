#pragma once

#include <iostream>
#include "Triangle.h"
#include <vector>

struct SceneObject {
	std::vector<Triangle *> triangles;
	SceneObject();
	SceneObject(std::vector<Triangle *> tris_init);
};
