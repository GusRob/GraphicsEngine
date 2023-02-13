#pragma once

#include <iostream>
#include "Triangle.h"
#include <vector>

struct SceneObject {
	std::string name;
	std::vector<Triangle *> triangles;

	Vector centerMass;
	float collisionSphereRadius;
	Material * material;
	SceneObject();
	SceneObject(std::vector<Triangle *> tris_init);
	void calcCollisionSphere();
};
