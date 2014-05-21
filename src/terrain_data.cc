// Copyright (c) 2014, Tamas Csala

#include "terrain_data.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include <GL/glew.h>
#include <Magick++.h>

using namespace std;

// -------======{[ Handling endianness ]}======-------

// The rtd is encoded in little-endian format.
// For big-endian users, conversion happens at both
// saving an loading.

/// Returns if this program is run on a LittleEndian computer.
static bool IsLittleEndian() {
  // The short value 1 has bytes (1, 0) in
  // little-endian and (0, 1) in big-endian
  short s = 1;
  return (((char*)&s)[0]) == 1;
}

/// Stores if this program is run on a LittleEndian computer.
static bool littleEndian = IsLittleEndian();

template <class T>
/// Performs an endian swap, if this computer is big-endian.
static void EndianSwap(T& data) {
  if (littleEndian) {
    return;
  }
  T swapped;
  for (size_t i = 0; i < sizeof(T); i++) {
    ((char *)&swapped)[i] = ((char*)&data)[sizeof(T) - i - 1];
  }
  data = swapped;
}


// -------======{[ RawTerrainData ]}======-------

/// Constructs an empty terrianData (full of memory junk).
/** @param w - The width of the terrain data to create.
  * @param h - The height of the terrain data to create.
  * @param level - The mipmap level of the terrain data. */
RawTerrainData::RawTerrainData(int w, int h, int level)
    : w(w), h(h), level(level) {
  heightData.resize(w*h);
}

/// Loads in a terrain data, from either an .rtd, a heightmap (image), or a DEM (.asc).
/** @param filename - The path to the file to load. */
RawTerrainData::RawTerrainData(const std::string& filename) : level(0) {
  size_t rtd_pos = filename.find(".rtd");
  if (rtd_pos != string::npos) {
    // Get the number at the end of the extension. It's the mipmap level.
    level = atoi(filename.c_str() + rtd_pos + 4);

    if (level > MAX_NUM_TERRAIN_MIPMAPS) {
      throw std::runtime_error("Invalid filename, too high mipmap level: " + filename);
    }

    // Check if the that mipmap level exists.
    ifstream file(filename);
    if (file.is_open()) {
      initFromRawTerrain(filename);
    } else {
      // Check if we have mipmap 0.
      std::string base_path = filename.substr(0, rtd_pos + 4);
      std::string rtd0_path = base_path + '0';
      file.open(rtd0_path);
      if (file.is_open()) {
        // If we do, then generate the mipmaps.
        level = 0;
        initFromRawTerrain(rtd0_path);
        createLoDs(base_path, MAX_NUM_TERRAIN_MIPMAPS);
        // Then load the level we actually wanted.
        initFromRawTerrain(filename);
      } else {
        throw std::runtime_error("Can't find file: " + filename);
      }
    }

  } else if (filename.find(".asc") != string::npos) {
    initFromAsc(filename);
  } else {
    initFromImage(filename);
  }
}

/// Saves the rtd in .rtd or an image format.
void RawTerrainData::save(const std::string& filename) const {
  if(filename.find(".rtd") != std::string::npos) {
    ofstream ofs(filename, ios::binary);

    size_t temp_w = w, temp_h = h;
    EndianSwap(temp_w);
    ofs.write((const char *)&temp_w, sizeof(size_t));
    EndianSwap(temp_h);
    ofs.write((const char *)&temp_h, sizeof(size_t));
    ofs.write((const char *)heightData.data(), heightData.size());

    assert(ofs.good());
  } else {
    Magick::Image image;
    image.read(w, h, "R", MagickCore::CharPixel, heightData.data());
    image.write(filename);
  }
}

/// Returns the next mipmap level of this rtd.
std::unique_ptr<RawTerrainData> RawTerrainData::getNextLodLevel() const {
 std::unique_ptr<RawTerrainData> rtd{ new RawTerrainData(w/2, h/2, level + 1) };
  for (size_t x = 0; x < w/2; x++) {
    for (size_t y = 0; y < h/2; y++) {
      (*rtd)(x, y) = ((short)(*this)(2*x, 2*y) + (*this)(2*x + 1, 2*y) +
                  (*this)(2*x, 2*y + 1) + (*this)(2*x + 1, 2*y + 1)) / 4;
    }
  }
  return rtd;
}

/// Creates a given number of mipmaps for this rtd.
/** @param filename - The path of the file, where to save the mipmaps. Should end with '.rtd'.
  * @param num_level - The number of mipmap levels to generate. */
void RawTerrainData::createLoDs(const std::string& filename, int num_levels) const {
  if (num_levels <= 0) {
    return;
  }

  std::unique_ptr<RawTerrainData> r{ getNextLodLevel() };
  std::stringstream ss;
  ss << r->level;
  r->save(filename + ss.str());
  r->createLoDs(filename, num_levels - 1);
}

/// Indexes the terrain data like it was 2D array.
const unsigned char& RawTerrainData::operator()(size_t x, size_t y) const {
  if (x < w && y < h) {
    return heightData[y*w + x];
  } else {
    throw std::out_of_range("OverIndexing in RawTerrainData::operator()");
  }
}

/// Indexes the terrain data like it was 2D array.
unsigned char& RawTerrainData::operator()(size_t x, size_t y) {
  if (x < w && y < h) {
    return heightData[y*w + x];
  } else {
    throw std::out_of_range("OverIndexing in RawTerrainData::operator()");
  }
}

bool RawTerrainData::isValid(size_t x, size_t y) const {
  return x < w && y < h;
}

unsigned char RawTerrainData::get(size_t x, size_t y) const {
  if (x < w && y < h) {
    return heightData[y*w + x];
  } else {
    return 0;
  }
}

/// Loads in a terrain data, from a '.rtd' file.
/** @param filename - The path to the file to load. */
void RawTerrainData::initFromRawTerrain(const std::string& filename) {
  ifstream ifs(filename, ios::binary);
  if (!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }

  ifs.read((char *)&w, sizeof(size_t));
  EndianSwap(w);
  ifs.read((char *)&h, sizeof(size_t));
  EndianSwap(h);

  heightData.resize(w * h);
  ifs.read((char *)heightData.data(), heightData.size());

  if (!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }
}

/// Loads in a terrain data, from a '.asc' (DEM) file.
/** @param filename - The path to the file to load. */
void RawTerrainData::initFromAsc(const std::string& filename) {
  ifstream ifs(filename);
  if (!ifs.good()) {
    throw std::runtime_error("Error reading file: " + filename);
  }
  string str;
  GLushort iData;
  double dData;

  ifs >> str >> iData;
  assert(str == "ncols");
  w = iData;

  ifs >> str >> iData;
  assert(str == "nrows");
  h = iData;

  // Some data we don't need to store
  ifs >> str >> dData;
  assert(str == "xllcorner");
  ifs >> str >> dData;
  assert(str == "yllcorner");
  ifs >> str >> dData;
  assert(str == "cellsize");

  ifs >> str >> iData;
  assert(str == "NODATA_value");
  int noData = iData;

  std::vector<GLushort> ushort_heightData;
  ushort_heightData.reserve(w * h);
  GLushort max = 0;
  for (size_t i = 0; i < w * h; i++) {
    ifs >> iData;
    assert(iData != noData);
    ushort_heightData.push_back(iData);
    if (iData > max) {
      max = iData;
    }
  }

  heightData.reserve(w * h);
  for (size_t i = 0; i < w * h; i++) {
    unsigned char c = ushort_heightData[i] * 255. / max;
    heightData.push_back(c);
  }
}

/// Loads in a terrain data, from an image file.
/** Only the red channel will be used.
  * @param filename - The path to the file to load. */
void RawTerrainData::initFromImage(const std::string& filename) {
  Magick::Image image(filename);
  w = image.columns();
  h = image.rows();
  heightData.resize(w * h);
  image.write(0, 0, w, h, "R", MagickCore::CharPixel, heightData.data());
}
