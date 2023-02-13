#include "Triangle.h"

Triangle::Triangle(){}

Triangle::Triangle(Vector p0_init, Vector p1_init, Vector p2_init, Material *mat_init) : p0(p0_init), p1(p1_init), p2(p2_init), mat(mat_init) {}

Triangle::Triangle(Vector p[], Material *mat_init) : mat(mat_init) {
	p0 = p[0]; p1 = p[1]; p2 = p[2];
}

Triangle::Triangle(Vector p[], Vector t[], Material *mat_init) : mat(mat_init) {
	p0 = p[0]; p1 = p[1]; p2 = p[2];
	t0 = t[0]; t1 = t[1]; t2 = t[2];
}

std::ostream &operator<<(std::ostream &os, const Triangle &tri) {
	os << "Triangle(pos("
	<< tri.p0
	<< ", "
	<< tri.p1
	<< ", "
	<< tri.p2
	<< "), tex("
	<< tri.t0
	<< ", "
	<< tri.t1
	<< ", "
	<< tri.t2
	<< "), "
	<< *tri.mat
	<< ")";
	return os;
}
