#include "Matrix.h"
#include "Vector.h"
#include <math.h>

Matrix::Matrix(float init){
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if(i == j){
        data[i][j] = init;
      } else {
        data[i][j] = 0;
      }
    }
  }
}

Matrix::Matrix(float init[][3]){
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      data[i][j] = init[i][j];
    }
  }
}

Matrix::Matrix(Vector p0, Vector p1, Vector p2){
  data[0][0] = p0.x;
  data[0][1] = p0.y;
  data[0][2] = p0.z;
  data[1][0] = p1.x;
  data[1][1] = p1.y;
  data[1][2] = p1.z;
  data[2][0] = p2.x;
  data[2][1] = p2.y;
  data[2][2] = p2.z;
}

std::ostream &operator<<(std::ostream &os, const Matrix &m) {
  os << "Matrix(("
  << m.data[0][0]
  << ", "
  << m.data[0][1]
  << ", "
  << m.data[0][2]
  << "), ("
  << m.data[1][0]
  << ", "
  << m.data[1][1]
  << ", "
  << m.data[1][2]
  << "), ("
  << m.data[2][0]
  << ", "
  << m.data[2][1]
  << ", "
  << m.data[2][2]
  << ")";
  return os;
}

Vector operator*(Matrix m, Vector v){
  float x = m.data[0][0]*v.x + m.data[0][1]*v.y + m.data[0][2]*v.z;
  float y = m.data[1][0]*v.x + m.data[1][1]*v.y + m.data[1][2]*v.z;
  float z = m.data[2][0]*v.x + m.data[2][1]*v.y + m.data[2][2]*v.z;
  return Vector(x, y, z);
}

Matrix inverse(Matrix mat){
  Matrix result = Matrix(0.0);
  float determinant = 0;
  //finding determinant
  for(int i = 0; i < 3; i++){
    determinant = determinant + (mat.data[0][i] * (mat.data[1][(i+1)%3] * mat.data[2][(i+2)%3] - mat.data[1][(i+2)%3] * mat.data[2][(i+1)%3]));
  }
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      result.data[i][j] = ((mat.data[(j+1)%3][(i+1)%3] * mat.data[(j+2)%3][(i+2)%3]) - (mat.data[(j+1)%3][(i+2)%3] * mat.data[(j+2)%3][(i+1)%3])) / determinant;
    }
  }
  return result;
}

Matrix transpose(Matrix m){
  return Matrix({{m.data[0][0], m.data[1][0], m.data[2][0]}, {m.data[0][1], m.data[1][1], m.data[2][1]}, {m.data[0][2], m.data[1][2], m.data[2][2]}});
}
