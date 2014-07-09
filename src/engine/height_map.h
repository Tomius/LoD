// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_HEIGHT_MAP_H_
#define ENGINE_HEIGHT_MAP_H_

#include <climits>
#include "../oglwrap/debug/insertion.h"
#include "./transform.h"
#include "./height_map_interface.h"
#include "./texture_source.h"

namespace engine {

template<typename T>
class HeightMap : public HeightMapInterface {
  TextureSource<T, 1> tex_;

 public:
  // Loads in a texture from a file
  // The format string may contain any of these two flags:
  // - 'C': a compressed image will be used.
  // - 'I': an integer image will be used.
  HeightMap(const std::string& file_name,
            const std::string& format_string = "CR")
      : tex_(file_name, format_string) {
    static_assert(std::is_same<T, char>::value ||
                  std::is_same<T, unsigned char>::value ||
                  std::is_same<T, short>::value ||
                  std::is_same<T, unsigned short>::value,
                  "Only uchar and ushort heightmaps are supported yet");
  }

  // The width and height of the texture
  virtual int w() const override { return tex_.w(); }
  virtual int h() const override { return tex_.h(); }

  virtual glm::vec2 extent() const override {
    return glm::vec2(w(), h());
  }

  virtual glm::vec2 center() const override {
    return extent()/2.0f;
  }

  virtual bool valid(double s, double t) const override {
    return tex_.valid(s, t);
  }

  virtual double heightAt(int s, int t) const override {
    return tex_(s, t)[0] / double(std::numeric_limits<T>::max()) * 255;
  }

  virtual double heightAt(double s, double t) const override {
    /*
     * fs, ct -- cs, ct
     *    |        |
     *    |        |
     * fs, ft -- cs, ft
     */

    double fs = floor(s), cs = fs + 1;
    double ft = floor(t), ct = ft + 1;

    double fh = glm::mix(double(tex_(fs, ft)[0]), double(tex_(cs, ft)[0]), s-fs);
    double ch = glm::mix(double(tex_(fs, ct)[0]), double(tex_(cs, ct)[0]), s-fs);

    return glm::mix(fh, ch, t-ft) / double(std::numeric_limits<T>::max()) * 255;
  }

  virtual gl::PixelDataFormat format() const override {
    return tex_.format();
  }

  virtual gl::PixelDataType type() const override {
    return tex_.type();
  }

  virtual void upload(gl::Texture2D& tex) const override {
    tex_.upload(tex);
  }

  virtual const void* data() const override {
    return tex_.data().data();
  }
};

}  // namespace engine

#endif
