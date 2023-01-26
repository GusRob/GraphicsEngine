#pragma once

#include <iostream>

struct Point2D {
	float x{};
	float y{};
	Point2D(float x_init, float y_init);
};

std::ostream &operator<<(std::ostream &os, const Point2D &point);
