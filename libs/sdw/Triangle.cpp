#include "Triangle.h"

Triangle::Triangle(Vector p0_init, Vector p1_init, Vector p2_init, Colour col_init) : p0(p0_init), p1(p1_init), p2(p2_init), col(col_init) {}

std::ostream &operator<<(std::ostream &os, const Triangle &tri) {
	os << "Triangle("
	   << tri.p0
     << ", "
	   << tri.p1
     << ", "
       << tri.p2
     << ")";
	return os;
}
