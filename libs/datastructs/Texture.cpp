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

std::vector<uint32_t> Texture::readPPM(std::string fp){
  std::ifstream inputStream(fp, std::ifstream::binary);
  std::string nextLine;
  std::getline(inputStream, nextLine);
  std::getline(inputStream, nextLine);
  std::vector<uint32_t> result;
  while(nextLine.at(0) == '#') std::getline(inputStream, nextLine);
  int middle = nextLine.find(' ');
  width = std::stoi(nextLine.substr(0, middle));
  height = std::stoi(nextLine.substr(middle));
  std::getline(inputStream, nextLine);
  result.resize(width*height);
  for(size_t i = 0; i < width*height; i++){
    int red = inputStream.get();
    int green = inputStream.get();
    int blue = inputStream.get();
    result[i] = ((255 << 24) + (red << 16) + (green << 8) + blue);
  }
  inputStream.close();
  return result;
}

void Texture::addDiffuseMap(std::string filepath){
  diffuseMap = readPPM(filepath);
}
void Texture::addHeightMap(std::string filepath){
  heightMap = readPPM(filepath);
}
void Texture::addNormalMap(std::string filepath){
  normalMap = readPPM(filepath);
}
void Texture::addRoughnessMap(std::string filepath){
  heightMap = readPPM(filepath);
}
void Texture::addAmboccMap(std::string filepath){
  amboccMap = readPPM(filepath);
}
