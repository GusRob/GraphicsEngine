#pragma once

#include <iostream>
#include "Colour.h"
#include "Vector.h"

struct Light {
	Vector position;
	Colour tint;
  Light();
  Light(Vector pos_init);
  Light(Vector pos_init, Colour tint_init);
};

std::ostream &operator<<(std::ostream &os, const Light &light);
