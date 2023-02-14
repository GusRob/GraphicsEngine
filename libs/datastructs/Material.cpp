#include "Material.h"

Material::Material(){
  col = Colour();
  texture = Texture();
}

Material::Material(Colour col_init){
  col = col_init;
  texture = Texture();
}

Material::Material(Colour col_init, Texture tex_init){
  col = col_init;
  texture = tex_init;
}

Material::Material(std::string name_init, Colour col_init, Texture tex_init){
  name = name_init;
  col = col_init;
  texture = tex_init;
  if(texture.width > 0){
    textureSet = true;
  }
}

std::ostream &operator<<(std::ostream &os, const Material &m) {
  os << "Material(("
  << m.col
  << ", "
  << m.texture
  << ")";
  return os;
}
