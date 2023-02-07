#pragma once
#pragma once

#include <iostream>

struct Point3D {
	float x{};
	float y{};
    float z{};
	Point3D(float x_init, float y_init, float z_init);
	Point3D();
};

std::ostream &operator<<(std::ostream &os, const Point3D &point);

Point3D operator-(Point3D p1, Point3D p2);
Point3D operator+(Point3D p1, Point3D p2);
Point3D operator-(Point3D p);
