// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TEXTURE_H_
#define ENGINE_TEXTURE_H_

#include <string>
#include "../lod_oglwrap_config.h"
#include "../oglwrap/textures/texture_2D.h"
#include "../oglwrap/context.h"

namespace engine {

template<typename DATA_TYPE, char NUM_COMPONENTS>
class Texture {
  bool integer_;
  std::string format_string_;
  std::vector<std::array<DATA_TYPE, NUM_COMPONENTS>> data_;
  int w_, h_;
public:
  // Loads in a texture from a file
  // Set integer to true, if you want to fetch texture values as integers
  Texture(const std::string& file_name,
          const std::string& format_string = "RGBA",
          bool integer = false);

  virtual ~Texture() {}

  // getters
  int w() const {return w_;}
  int h() const {return h_;}
  bool integer() const {return integer_;}
  void set_integer(bool integer) {integer_ = integer;}
  std::vector<std::array<DATA_TYPE, NUM_COMPONENTS>>& data() {
    return data_;
  }
  const std::vector<std::array<DATA_TYPE, NUM_COMPONENTS>>& data() const {
    return data_;
  }
  const std::string& format_string() const {return format_string_;}

  // Returns the format of the height data
  virtual oglwrap::PixelDataFormat format() const;

  // Returns the default internal-format for this format.
  virtual oglwrap::PixelDataInternalFormat internalFormat() const;

  // Returns the type of the height data
  virtual oglwrap::PixelDataType type() const;

  // Indexes the array, but doesn't care about over or under-indexing
  std::array<DATA_TYPE, NUM_COMPONENTS>& operator()(int x, int y) {
    return data_[y*h_ + x];
  }
  const std::array<DATA_TYPE, NUM_COMPONENTS>& operator()(int x, int y) const {
    return data_[y*h_ + x];
  }

  // Indexes the array, throws at over or under-indexing
  std::array<DATA_TYPE, NUM_COMPONENTS>& at(int x, int y) {
    return data_.at(y*h_ + x);
  }
  const std::array<DATA_TYPE, NUM_COMPONENTS>& at(int x, int y) const {
    return data_.at(y*h_ + x);
  }

  // Returns if the coordinates are valid
  virtual bool valid(double x, double y) const {
    return 0 < x && x < w_ && 0 < y && y < h_;
  }

  // Uploads it to a texture object
  virtual void upload(oglwrap::Texture2D& tex) const;
  virtual void upload(oglwrap::Texture2D& tex,
                      oglwrap::PixelDataInternalFormat internal_format) const;
};

}

#include "texture-inl.h"

#endif
