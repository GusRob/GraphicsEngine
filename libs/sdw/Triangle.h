#pragma once
#pragma once

#include <iostream>
#include "Colour.h"
#include "Vector.h"

struct Triangle {
	Vector p1{};
	Vector p2{};
    Vector p3{};
    Colour col{};
	Triangle(Vector p1_init, Vector p2_init, Vector p3_init, Colour col_init);
};

std::ostream &operator<<(std::ostream &os, const Triangle &tri);
