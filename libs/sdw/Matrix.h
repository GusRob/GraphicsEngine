#pragma once

#include <iostream>
#include "Vector.h"

struct Matrix {
	float data[3][3];
	Matrix(float init);    
	Matrix(float init[][3]);    
};

std::ostream &operator<<(std::ostream &os, const Matrix &mat);

Vector operator*(Matrix m, Vector v);