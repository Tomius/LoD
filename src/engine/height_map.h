// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_BASIC_HEIGHT_MAP_H_
#define ENGINE_BASIC_HEIGHT_MAP_H_

#include "height_map_interface.h"
#include "texture_source.h"

namespace engine {

template<typename DATA_TYPE>
class HeightMap : public HeightMapInterface {
  TextureSource<DATA_TYPE, 1> tex_;
public:
  // Loads in a texture from a file
  // The format string may contain any of these two flags:
  // - 'C': a compressed image will be used.
  // - 'I': an integer image will be used.
  HeightMap(const std::string& file_name,
            const std::string& format_string = "CR")
  : tex_(file_name, format_string) {}

  // The width and height of the texture
  virtual int w() const override { return tex_.w(); }
  virtual int h() const override { return tex_.h(); }

  // Returns if the coordinates are valid
  virtual bool valid(double x, double y) const override { return tex_.valid(x, y); }

  virtual double heightAt(int x, int y) const override { return tex_(x, y)[0]; }

  virtual double heightAt(double x, double y) const override {
    /*
     * fx, cy -- cx, cy
     *    |        |
     *    |        |
     * fx, fy -- cx, fy
     */

    double fx = floor(x), cx = ceil(x);
    double fy = floor(y), cy = ceil(y);

    double fh = glm::mix(double(tex_(fx, fy)[0]), double(tex_(cx, fy)[0]), x-fx);
    double ch = glm::mix(double(tex_(fx, cy)[0]), double(tex_(cx, cy)[0]), x-fx);

    return glm::mix(fh, ch, y-fy);
  }

  virtual gl::PixelDataFormat format() const override {
    return tex_.format();
  }

  virtual gl::PixelDataType type() const override {
    return tex_.type();
  }

  virtual void upload(gl::Texture2D& tex) const {
    tex_.upload(tex);
  }
};


}


#endif
