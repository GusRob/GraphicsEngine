#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <vector>

struct Texture {
  std::vector<uint32_t> diffuseMap;
  std::vector<uint32_t> normalMap;
  std::vector<uint32_t> heightMap;
  std::vector<uint32_t> roughnessMap;
  std::vector<uint32_t> amboccMap;

  std::string filepath;

  size_t width;
  size_t height;

  Texture(std::string filepath);
  Texture();
  std::vector<uint32_t> readPPM(std::string filepath);

  void addDiffuseMap(std::string filepath);
  void addHeightMap(std::string filepath);
  void addNormalMap(std::string filepath);
  void addRoughnessMap(std::string filepath);
  void addAmboccMap(std::string filepath);

};

std::ostream &operator<<(std::ostream &os, const Texture &t);
