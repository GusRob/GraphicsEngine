#include "Light.h"

Light::Light(){}

Light::Light(Vector pos_init){
  position = pos_init;
  tint = Colour(255, 255, 255);
}

Light::Light(Vector pos_init, Colour tint_init){
  position = pos_init;
  tint = tint_init;
}

std::ostream &operator<<(std::ostream &os, const Light &light) {
	os << "Light("
	<< light.position
	<< ", "
	<< light.tint
	<< ")";
	return os;
}
