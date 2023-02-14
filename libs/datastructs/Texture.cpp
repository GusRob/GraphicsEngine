#include "Texture.h"

std::ostream &operator<<(std::ostream &os, const Texture &t){
  os << "Texture(("
  << t.width
  << ", "
  << t.height
  << "), "
  << t.filepath
  << ")";
  return os;
}

Texture::Texture(){
  width = 0;
  height = 0;
}

Texture::Texture(std::string filepath_init){
  addDiffuseMap(filepath_init);
}

void Texture::addDiffuseMap(std::string filepath_init){
  filepath = filepath_init;
  std::ifstream inputStream(filepath, std::ifstream::binary);
  std::string nextLine;
  std::getline(inputStream, nextLine);
  std::getline(inputStream, nextLine);
  while(nextLine.at(0) == '#') std::getline(inputStream, nextLine);
  int middle = nextLine.find(' ');
  width = std::stoi(nextLine.substr(0, middle));
  height = std::stoi(nextLine.substr(middle));
  std::getline(inputStream, nextLine);
  diffuseMap.resize(width*height);
  for(size_t i = 0; i < width*height; i++){
    int red = inputStream.get();
    int green = inputStream.get();
    int blue = inputStream.get();
    diffuseMap[i] = ((255 << 24) + (red << 16) + (green << 8) + blue);
  }
  inputStream.close();

}
