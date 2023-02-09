#include "Vector.h"

Vector::Vector(){}

Vector::Vector(float x_init, float y_init, float z_init) : x(x_init), y(y_init), z(z_init) {dim=3;}

Vector::Vector(float x_init, float y_init) : x(x_init), y(y_init){dim=2;z=0;}

std::ostream &operator<<(std::ostream &os, const Vector &point) {
	os << "Vector("
	   << point.x
     << ", "
	   << point.y
     << ", "
       << point.z
     << ")";
	return os;
}


Vector operator-(Vector p1, Vector p2){
    return Vector(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);
}

Vector operator+(Vector p1, Vector p2){
    return Vector(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}

Vector operator-(Vector p){
    return Vector(-p.x, -p.y, -p.z);
}