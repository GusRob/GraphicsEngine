#include "Face3D.h"
#include "Point3D.h"
#include "Colour.h"

Face3D::Face3D(Point3D p1_init, Point3D p2_init, Point3D p3_init, Colour col_init) : p1(p1_init), p2(p2_init), p3(p3_init), col(col_init) {}

std::ostream &operator<<(std::ostream &os, const Face3D &face) {
	os << "Face3D("
	   << face.p1
     << ", "
	   << face.p2
     << ", "
       << face.p3
     << ")";
	return os;
}
