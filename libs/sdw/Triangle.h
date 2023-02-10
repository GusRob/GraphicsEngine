#pragma once
#pragma once

#include <iostream>
#include "Colour.h"
#include "Vector.h"

struct Triangle {
	Vector p0{};
	Vector p1{};
	Vector p2{};
	Colour col{};
	Triangle(Vector p0_init, Vector p1_init, Vector p2_init, Colour col_init);
	Triangle();
};

std::ostream &operator<<(std::ostream &os, const Triangle &tri);
