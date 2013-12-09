#include <fstream>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <Magick++.h>
#include "terrainData.hpp"

using namespace std;

// -------======{[ Handling endianness ]}======-------

// All the binary formats described here use little-endian encoding.

bool IsLittleEndian() {
  // The short value 1 has bytes (1, 0) in
  // little-endian and (0, 1) in big-endian
  short s = 1;
  return (((char*)&s)[0]) == 1;
}

static bool littleEndian = IsLittleEndian();

template <class T>
void endianSwap(T& data) {
  if(littleEndian) {
    return;
  }
  T swapped;
  for(size_t i = 0; i < sizeof(T); i++) {
    ((char *)&swapped)[i] = ((char*)&data)[sizeof(T) - i - 1];
  }
  data = swapped;
}


// -------======{[ RawTerrainData ]}======-------

RawTerrainData::RawTerrainData(const std::string& filename) {
  if(filename.find(".rtd") != string::npos) {
    InitFromRawTerrain(filename);
  } else if(filename.find(".asc") != string::npos) {
    InitFromAsc(filename);
  } else {
    InitFromImage(filename);
  }
}

void RawTerrainData::Save(const std::string& filename) {
  ofstream ofs(filename, ios::binary);

  endianSwap(w);
  ofs.write((const char *)&w, sizeof(size_t));
  endianSwap(h);
  ofs.write((const char *)&h, sizeof(size_t));
  ofs.write((const char *)heightData.data(), heightData.size());

  assert(ofs.good());
}

void RawTerrainData::InitFromRawTerrain(const std::string& filename) {
  ifstream ifs(filename, ios::binary);
  if(!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }

  ifs.read((char *)&w, sizeof(size_t));
  endianSwap(w);
  ifs.read((char *)&h, sizeof(size_t));
  endianSwap(h);

  heightData.resize(w * h);
  ifs.read((char *)heightData.data(), heightData.size());

  if(!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }
}

void RawTerrainData::InitFromAsc(const std::string& filename) {
  ifstream ifs(filename);
  if(!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }
  string str;
  unsigned short iData;
  double dData;

  ifs >> str >> iData;
  assert(str == "ncols");
  w = iData;

  ifs >> str >> iData;
  assert(str == "nrows");
  h = iData;

  // Some data I don't need
  ifs >> str >> dData;
  assert(str == "xllcorner");
  ifs >> str >> dData;
  assert(str == "yllcorner");
  ifs >> str >> dData;
  assert(str == "cellsize");

  ifs >> str >> iData;
  assert(str == "NODATA_value");
  int noData = iData;

  std::vector<unsigned short> ushort_heightData;
  ushort_heightData.reserve(w * h);
  unsigned short max = 0;
  for(size_t i = 0; i < w * h; i++) {
    ifs >> iData;
    assert(iData != noData);
    ushort_heightData.push_back(iData);
    if(iData > max) {
      max = iData;
    }
  }

  heightData.reserve(w * h);
  for(size_t i = 0; i < w * h; i++) {
    unsigned char c = ushort_heightData[i] * 255. / max;
    heightData.push_back(c);
  }
}

void RawTerrainData::InitFromImage(const std::string& filename) {
  Magick::Image image(filename);
  w = image.columns();
  h = image.rows();
  heightData.resize(w * h);
  image.write(0, 0, w, h, "R", MagickCore::CharPixel, heightData.data());
}
