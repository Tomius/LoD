// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TEXTURE_SOURCE_H_
#define ENGINE_TEXTURE_SOURCE_H_

#include <array>
#include <string>
#include <vector>

#include "./oglwrap_config.h"
#include "../oglwrap/textures/texture_2D.h"
#include "../oglwrap/context.h"

namespace engine {

template<typename T, char NUM_COMPONENTS>
class TextureSource {
  bool integer_, srgb_, compressed_;
  std::string format_string_;
  std::vector<std::array<T, NUM_COMPONENTS>> data_;
  int w_, h_;

 public:
  // Loads in a texture from a file
  // The format string can contain any of these flags:
  // - 'S': the image is converted from SRGB to linear colorspace at load.
  // - 'C': a compressed image will be used.
  // - 'I': an integer image will be used.
  TextureSource(const std::string& file_name,
                std::string format_string = "CSRGBA");

  virtual ~TextureSource() {}

  // getters
  int w() const {return w_;}
  int h() const {return h_;}
  bool integer() const {return integer_;}
  void set_integer(bool integer) {integer_ = integer;}
  std::vector<std::array<T, NUM_COMPONENTS>>& data() {
    return data_;
  }
  const std::vector<std::array<T, NUM_COMPONENTS>>& data() const {
    return data_;
  }
  const std::string& format_string() const {return format_string_;}

  // Returns the format of the height data
  virtual gl::PixelDataFormat format() const;

  // Returns the default internal-format for this format.
  virtual gl::PixelDataInternalFormat internalFormat() const;

  // Returns the type of the height data
  virtual gl::PixelDataType type() const;

  // Indexes the array, but doesn't care about over or under-indexing
  std::array<T, NUM_COMPONENTS>& operator()(int x, int y) {
    return data_[y*h_ + x];
  }
  const std::array<T, NUM_COMPONENTS>& operator()(int x, int y) const {
    return data_[y*h_ + x];
  }

  // Indexes the array, throws at over or under-indexing
  std::array<T, NUM_COMPONENTS>& at(int x, int y) {
    return data_.at(y*h_ + x);
  }
  const std::array<T, NUM_COMPONENTS>& at(int x, int y) const {
    return data_.at(y*h_ + x);
  }

  // Returns if the coordinates are valid
  virtual bool valid(double x, double y) const {
    return 0 < x && x < w_ && 0 < y && y < h_;
  }

  // Uploads it to a texture object
  virtual void upload(gl::Texture2D& tex) const;
  virtual void upload(gl::Texture2D& tex,
                      gl::PixelDataInternalFormat internal_format) const;
};

}  // namespace engine

#include "./texture_source-inl.h"

#endif
