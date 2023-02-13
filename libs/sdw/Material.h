#pragma once

#include <iostream>
#include "Colour.h"
#include "Texture.h"

struct Material {
	Colour col;
	Texture texture;
	Material();
	Material(Colour col_init);
	Material(Colour col_init, Texture tex_init);
};

std::ostream &operator<<(std::ostream &os, const Material &mat);
