#pragma once
#pragma once

#include <iostream>
#include "Colour.h"
#include "Vector.h"
#include "Material.h"

struct Triangle {
	Vector p0{};
	Vector p1{};
	Vector p2{};

	Vector t0{};
	Vector t1{};
	Vector t2{};

	Material *mat{};
	Triangle(Vector p0_init, Vector p1_init, Vector p2_init, Material *mat_init);
	Triangle(Vector p_init[], Material *mat_init);
	Triangle(Vector p_init[], Vector t_init[], Material *mat_init);
	Triangle();
};

std::ostream &operator<<(std::ostream &os, const Triangle &tri);
