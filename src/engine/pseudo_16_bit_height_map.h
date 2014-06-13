// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_PSEUDO_16_BIT_HEIGHT_MAP_H_
#define ENGINE_PSEUDO_16_BIT_HEIGHT_MAP_H_

#include "height_map_interface.h"
#include "texture_source.h"

namespace engine {

class Pseudo16BitHeightMap : public HeightMapInterface {
  int w_, h_;
  std::vector<unsigned short> data_;

  unsigned short& fetch(int x, int y) {
    return data_[y*h_ + x];
  }

  const unsigned short& fetch(int x, int y) const {
    return data_[y*h_ + x];
  }

  double dfetch(int x, int y) const {
    return data_[y*h_ + x] / 65535.0 * 255.0;
  }

public:
  // Loads in a texture from a file
  Pseudo16BitHeightMap(const std::string& file_name) {
    assert(sizeof(unsigned short) == 2);

    TextureSource<unsigned char, 1> tex{file_name, "R"};
    w_ = tex.w(); h_ = tex.h();
    data_.reserve(w_ * h_);

    for(int x = 0; x < w_; ++x) {
      for(int y = 0; y < h_; ++y) {
        fetch(x, y) = (tex(x, y)[0] << 8) | (rand() % 256);
      }
    }
  }

  // The width and height of the texture
  virtual int w() const override { return w_; }
  virtual int h() const override { return h_; }

  // Returns if the coordinates are valid
  virtual bool valid(double x, double y) const override {
    return 0 < x && x < w_ && 0 < y && y < h_;
  }

  virtual double heightAt(int x, int y) const override {
    return dfetch(x, y);
  }

  virtual double heightAt(double x, double y) const override {
    /*
     * fx, cy -- cx, cy
     *    |        |
     *    |        |
     * fx, fy -- cx, fy
     */

    double fx = floor(x), cx = fx + 1;
    double fy = floor(y), cy = fy + 1;

    double fh = glm::mix(dfetch(fx, fy), dfetch(cx, fy), x-fx);
    double ch = glm::mix(dfetch(fx, cy), dfetch(cx, cy), x-fx);

    return glm::mix(fh, ch, y-fy);
  }

  virtual gl::PixelDataFormat format() const override {
    return gl::kRed;
  }

  virtual gl::PixelDataType type() const override {
    return gl::kUnsignedShort;
  }

  virtual void upload(gl::Texture2D& tex) const override {
    bool bad_alignment = (w_ * sizeof(unsigned short)) % 4 != 0;
    GLint unpack_aligment;

    if(bad_alignment) {
      glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
      gl::PixelStore(gl::kUnpackAlignment, 1);
    }

    tex.upload(
      gl::kR16F,
      w_, h_,
      gl::kRed,
      gl::kUnsignedShort,
      data_.data()
    );

    if(bad_alignment) {
      gl::PixelStore(gl::kUnpackAlignment, unpack_aligment);
    }
  }
};

}

#endif
