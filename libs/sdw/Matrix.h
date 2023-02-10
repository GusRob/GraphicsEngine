#pragma once

#include <iostream>
#include "Vector.h"

struct Matrix {
	float data[3][3];
	Matrix(float init);    
	Matrix(float init[][3]);    
	Matrix(Vector p0, Vector p1, Vector p2);    
};

std::ostream &operator<<(std::ostream &os, const Matrix &mat);

Vector operator*(Matrix m, Vector v);