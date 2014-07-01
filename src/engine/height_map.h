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
  glm::vec3 offset_, scale_;

 public:
  // Loads in a texture from a file
  // The format string may contain any of these two flags:
  // - 'C': a compressed image will be used.
  // - 'I': an integer image will be used.
  HeightMap(const std::string& file_name,
            const engine::Transform& transform,
            const std::string& format_string = "CR")
      : tex_(file_name, format_string)
      , offset_(transform.pos())
      , scale_(transform.scale()) {
    static_assert(std::is_same<T, unsigned char>::value ||
                  std::is_same<T, unsigned short>::value,
                  "Only uchar and ushort heightmaps are supported yet");
  }

  // The width and height of the texture
  virtual int w() const override { return tex_.w(); }
  virtual int h() const override { return tex_.h(); }

  virtual glm::vec2 extent() const override {
    return glm::vec2(w()*scale_.x, h()*scale_.z);
  }

  virtual glm::vec2 center() const override {
    return glm::vec2(offset_.x, offset_.z) + extent()/2.0f;
  }

  virtual bool fetchValid(double s, double t) const override {
    return tex_.valid(s, t);
  }

  virtual double fetchHeightAt(int s, int t) const override {
    float height = tex_(s, t)[0] / (std::numeric_limits<T>::max()-1.0) * 255;
    return (height + offset_.y) * scale_.y;
  }

  virtual double fetchHeightAt(double s, double t) const override {
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

    double height = glm::mix(fh, ch, t-ft) / (std::numeric_limits<T>::max()-1) * 255;
    return (height + offset_.y) * scale_.y;
  }

  virtual glm::dvec2 toWorldSpace(double s, double t) const override{
    return glm::dvec2((s + offset_.x) * scale_.x, (t + offset_.z) * scale_.z);
  }

  virtual glm::dvec2 toModelSpace(double s, double t) const override {
    return glm::dvec2(s / scale_.x - offset_.x, t / scale_.z - offset_.z);
  }

  virtual glm::vec3 toWorldSpace(const glm::vec3& p) const {
    return p / scale_ - offset_;
  }

  virtual glm::vec3 toModelSpace(const glm::vec3& p) const {
    return p / scale_ - offset_;
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
