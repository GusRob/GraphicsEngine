#pragma once

#include <iostream>

struct Vector {
    int dim = 3; //only support for 1, 2, and 3 dims
	float x{};
	float y{};
    float z{}; //used as z-depth for canvas
	Vector(float x_init, float y_init, float z_init);
	Vector(float x_init, float y_init);
	Vector();
    
};

std::ostream &operator<<(std::ostream &os, const Vector &point);

Vector operator-(Vector p0, Vector p1);
Vector operator+(Vector p0, Vector p1);
Vector operator*(Vector p0, Vector p1);
Vector operator/(Vector p0, Vector p1);
Vector operator-(Vector p);

float size(Vector v);
