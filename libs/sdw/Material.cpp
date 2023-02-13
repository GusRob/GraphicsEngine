#include "Material.h"

Material::Material(){}

Material::Material(Colour col_init){
  col = col_init;
}

Material::Material(Colour col_init, Texture tex_init){
  col = col_init;
  texture = tex_init;
}

std::ostream &operator<<(std::ostream &os, const Material &m) {
  os << "Material(("
  << m.col
  << ", "
  << m.texture
  << ")";
  return os;
}
