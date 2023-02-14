#include "Vector.h"
#include <math.h>

Vector::Vector(){}

Vector::Vector(float x_init, float y_init, float z_init) : x(x_init), y(y_init), z(z_init) {dim=3;}

Vector::Vector(float x_init, float y_init) : x(x_init), y(y_init){dim=2;z=0;}

Vector::Vector(std::vector<float> init){
  dim=3;
  x = init[0];
  y = init[1];
  z = init[2];
}

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

Vector cross(Vector p0, Vector p1){
  float cx = p0.y*p1.z - p0.z*p1.y;
  float cy = p0.z*p1.x - p0.x*p1.z;
  float cz = p0.x*p1.y - p0.y*p1.x;
  return Vector(cx, cy, cz);
}

Vector norm(Vector p){
  float n = size(p);
  return Vector(p.x/n, p.y/n, p.z/n);
}

float size(Vector v){
  return sqrt( (v.x*v.x) + (v.y*v.y) + (v.z*v.z) );
}

//element-wise ops

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

Vector operator-(Vector p, float v){
  return Vector(p.x-v, p.y-v, p.z-v);
}

Vector operator+(Vector p, float v){
  return Vector(p.x+v, p.y+v, p.z+v);
}

Vector operator*(Vector p, float v){
  return Vector(p.x*v, p.y*v, p.z*v);
}

Vector operator/(Vector p, float v){
  return Vector(p.x/v, p.y/v, p.z/v);
}

Vector operator-(Vector p){
  return Vector(-p.x, -p.y, -p.z);
}
