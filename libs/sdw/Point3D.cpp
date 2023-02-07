#include "Point3D.h"

Point3D::Point3D(){}

Point3D::Point3D(float x_init, float y_init, float z_init) : x(x_init), y(y_init), z(z_init) {}

std::ostream &operator<<(std::ostream &os, const Point3D &point) {
	os << "Point3D("
	   << point.x
     << ", "
	   << point.y
     << ", "
       << point.z
     << ")";
	return os;
}


Point3D operator-(Point3D p1, Point3D p2){
    return Point3D(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);
}

Point3D operator+(Point3D p1, Point3D p2){
    return Point3D(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}

Point3D operator-(Point3D p){
    return Point3D(-p.x, -p.y, -p.z);
}