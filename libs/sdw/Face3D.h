#pragma once
#pragma once

#include <iostream>
#include "Colour.h"
#include "Point3D.h"

struct Face3D {
	Point3D p1{};
	Point3D p2{};
    Point3D p3{};
    Colour col{};
	Face3D(Point3D p1_init, Point3D p2_init, Point3D p3_init, Colour col_init);
};

std::ostream &operator<<(std::ostream &os, const Face3D &face);
