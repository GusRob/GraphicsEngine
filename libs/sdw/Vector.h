#pragma once
#pragma once

#include <iostream>

struct Vector {
    int dim = 3;
	float x{};
	float y{};
    float z{};
	Vector(float x_init, float y_init, float z_init);
	Vector(float x_init, float y_init);
	Vector();
};

std::ostream &operator<<(std::ostream &os, const Vector &point);

Vector operator-(Vector p1, Vector p2);
Vector operator+(Vector p1, Vector p2);
Vector operator-(Vector p);
