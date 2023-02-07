#include "Point2D.h"

Point2D::Point2D(float x_init, float y_init) : x(x_init), y(y_init) {}

std::ostream &operator<<(std::ostream &os, const Point2D &point) {
	os << "Point2D("
	   << point.x
     << ", "
	   << point.y
     << ")";
	return os;
}
