// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_HEIGHT_MAP_INTERFACE_H_
#define ENGINE_HEIGHT_MAP_INTERFACE_H_

#include "../lod_oglwrap_config.h"
#include "../oglwrap/textures/texture_2D.h"

namespace engine {

// An interface to get data from a heightmap
class HeightMapInterface {
public:
  virtual ~HeightMapInterface() {}

  // The width and height of the texture (not the size of the heightmap)
  virtual int w() const = 0;
  virtual int h() const = 0;

  virtual glm::vec2 extent() const = 0;
  virtual glm::vec2 center() const = 0;

  // Returns if the texture coordinates are valid
  virtual bool fetchValid(double x, double z) const = 0;

  // Texture space fetch
  virtual double fetchHeightAt(int s, int t) const = 0;

  // Texture space fetch with interpolation
  virtual double fetchHeightAt(double s, double t) const = 0;

  virtual glm::dvec2 toWorldSpace(double s, double t) const {
    return glm::dvec2(s, t);
  }

  virtual glm::dvec2 toModelSpace(double s, double t) const {
    return glm::dvec2(s, t);
  }

  virtual glm::vec3 toWorldSpace(const glm::vec3& p) const {
    return p;
  }

  virtual glm::vec3 toModelSpace(const glm::vec3& p) const {
    return p;
  }

  // Returns if the world space coordinates represent a valid texel
  bool valid(double x, double z) const {
    glm::dvec2 tc = toModelSpace(x, z);
    return fetchValid(tc.x, tc.y);
  }

  // World space texture fetch (world space x and z coordinates)
  double heightAt(int x, int z) const {
    glm::dvec2 tc = toModelSpace(x, z);
    return fetchHeightAt(tc.x, tc.y);
  }

  // World space texture fetch with interpolation
  double heightAt(double x, double z) const {
    glm::dvec2 tc = toModelSpace(x, z);
    return fetchHeightAt(tc.x, tc.y);
  }

  // Returns the format of the height data
  virtual gl::PixelDataFormat format() const = 0;

  // Returns the type of the height data
  virtual gl::PixelDataType type() const = 0;

  // Uploads the heightmap to a texture object
  virtual void upload(gl::Texture2D& tex) const = 0;

  // Returns dvec2{min, max} of area between (x-w/2, y-h/2) and (x+w/2, y+h/2)
  // it returns {0, 0} if the area requested doesn't contain a single valid value
  virtual glm::dvec2 getMinMaxOfArea(int x, int y, int w, int h) const;
};


}


#endif
