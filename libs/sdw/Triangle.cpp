#include "Triangle.h"
#include "Vector.h"
#include "Colour.h"

Triangle::Triangle(Vector p1_init, Vector p2_init, Vector p3_init, Colour col_init) : p1(p1_init), p2(p2_init), p3(p3_init), col(col_init) {}

std::ostream &operator<<(std::ostream &os, const Triangle &tri) {
	os << "Triangle("
	   << tri.p1
     << ", "
	   << tri.p2
     << ", "
       << tri.p3
     << ")";
	return os;
}
