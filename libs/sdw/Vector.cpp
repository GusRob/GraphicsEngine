#include "Vector.h"
#include <math.h>

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


Vector operator-(Vector p0, Vector p1){
    return Vector(p0.x-p1.x, p0.y-p1.y, p0.z-p1.z);
}

Vector operator+(Vector p0, Vector p1){
    return Vector(p0.x+p1.x, p0.y+p1.y, p0.z+p1.z);
}

Vector operator*(Vector p0, Vector p1){
    return Vector(p0.x*p1.x, p0.y*p1.y, p0.z*p1.z);
}

Vector operator/(Vector p0, Vector p1){
    return Vector(p0.x/p1.x, p0.y/p1.y, p0.z/p1.z);
}

Vector operator-(Vector p){
    return Vector(-p.x, -p.y, -p.z);
}

float size(Vector v){
    return sqrt( (v.x*v.x) + (v.y*v.y) + (v.z*v.z) );
}