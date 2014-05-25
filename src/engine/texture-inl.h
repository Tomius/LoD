// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TEXTURE_INL_H_
#define ENGINE_TEXTURE_INL_H_

#include "texture.h"
#include "../oglwrap/smart_enums.h"

namespace engine {

template<typename DATA_TYPE, char NUM_COMPONENTS>
Texture<DATA_TYPE, NUM_COMPONENTS>::Texture(const std::string& file_name,
                                            std::string format_string) {

  // Preprocess format_string: 'S', 'C' and 'I' have special meaning
  size_t s_pos = format_string.find('S');
  if(s_pos != std::string::npos) {
    srgb_ = true;
    format_string.erase(format_string.begin() + s_pos);
    assert(NUM_COMPONENTS >= 3); // only rgb and rgba can be in srgb
  }

  size_t c_pos = format_string.find('C');
  if(c_pos != std::string::npos) {
    compressed_ = true;
    format_string.erase(format_string.begin() + c_pos);
  }

  size_t i_pos = format_string.find('I');
  if(i_pos != std::string::npos) {
    integer_ = true;
    format_string.erase(format_string.begin() + i_pos);
  }

  format_string_ = format_string;

  assert(NUM_COMPONENTS <= 4);
  assert(format_string.length() == NUM_COMPONENTS);

  Magick::Image image(file_name);
  w_ = image.columns();
  h_ = image.rows();
  data_.resize(w_ * h_);

  MagickCore::StorageType type = MagickCore::UndefinedPixel;
  if (std::is_same<DATA_TYPE, char>::value ||
     std::is_same<DATA_TYPE, unsigned char>::value) {
    type = MagickCore::CharPixel;
  } else if (std::is_same<DATA_TYPE, short>::value ||
     std::is_same<DATA_TYPE, unsigned short>::value) {
    type = MagickCore::ShortPixel;
  } else if (std::is_same<DATA_TYPE, int>::value ||
     std::is_same<DATA_TYPE, unsigned int>::value) {
    type = MagickCore::IntegerPixel;
  } else if (std::is_same<DATA_TYPE, long>::value ||
     std::is_same<DATA_TYPE, unsigned long>::value) {
    type = MagickCore::LongPixel;
  } else if (std::is_same<DATA_TYPE, float>::value) {
    type = MagickCore::FloatPixel;
  } else if (std::is_same<DATA_TYPE, double>::value) {
    type = MagickCore::FloatPixel;
  } else {
    abort();
  }

  image.write(0, 0, w_, h_, format_string_, type, data_.data());
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
oglwrap::PixelDataFormat Texture<DATA_TYPE, NUM_COMPONENTS>::format() const {
  using gl = oglwrap::SmartEnums;

  if (integer_) {
    if (format_string_ == "R") {
      return gl::RedInteger;
    } else if (format_string_ == "G") {
      return gl::GreenInteger;
    } else if (format_string_ == "B") {
      return gl::BlueInteger;
    } else if (format_string_ == "RG") {
      return gl::RgInteger;
    } else if (format_string_ == "RGB") {
      return gl::RgbInteger;
    } else if (format_string_ == "RGBA") {
      return gl::RgbaInteger;
    } else if (format_string_ == "BGR") {
      return gl::BgrInteger;
    } else if (format_string_ == "BGRA") {
      return gl::BgraInteger;
    } else {
      abort();
    }
  } else {
    if (format_string_ == "R") {
      return gl::Red;
    } else if (format_string_ == "G") {
      return gl::Green;
    } else if (format_string_ == "B") {
      return gl::Blue;
    } else if (format_string_ == "RG") {
      return gl::Rg;
    } else if (format_string_ == "RGB") {
      return gl::Rgb;
    } else if (format_string_ == "RGBA") {
      return gl::Rgba;
    } else if (format_string_ == "BGR") {
      return gl::Bgr;
    } else if (format_string_ == "BGRA") {
      return gl::Bgra;
    } else {
      abort();
    }
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
oglwrap::PixelDataInternalFormat
Texture<DATA_TYPE, NUM_COMPONENTS>::internalFormat() const {
  using gl = oglwrap::SmartEnums;

  if(compressed_) {
    if (format_string_ == "R" || format_string_ == "G" || format_string_ == "B") {
      return gl::CompressedRed;
    } else if (format_string_ == "RG") {
      return gl::CompressedRg;
    } else if (format_string_ == "RGB" || format_string_ == "BGR") {
      return srgb_ ? gl::CompressedSrgb : gl::CompressedRgb;
    } else if (format_string_ == "RGBA" || format_string_ == "BGRA") {
      return srgb_ ? gl::CompressedSrgbAlpha : gl::CompressedRgba;
    } else {
      abort();
    }
  } else {
    if (format_string_ == "R" || format_string_ == "G" || format_string_ == "B") {
      return gl::Red;
    } else if (format_string_ == "RG") {
      return gl::Rg;
    } else if (format_string_ == "RGB" || format_string_ == "BGR") {
      return srgb_ ? gl::Srgb : gl::Rgb;
    } else if (format_string_ == "RGBA" || format_string_ == "BGRA") {
      return srgb_ ? gl::SrgbAlpha : gl::Rgba;
    } else {
      abort();
    }
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
oglwrap::PixelDataType Texture<DATA_TYPE, NUM_COMPONENTS>::type() const {
  using gl = oglwrap::SmartEnums;

  if (std::is_same<DATA_TYPE, char>::value) {
    return gl::Byte;
  } else if (std::is_same<DATA_TYPE, unsigned char>::value) {
    return gl::UnsignedByte;
  } else if (std::is_same<DATA_TYPE, short>::value) {
    return gl::Short;
  } else if (std::is_same<DATA_TYPE, unsigned short>::value) {
    return gl::UnsignedShort;
  } else if (std::is_same<DATA_TYPE, int>::value) {
    return gl::Int;
  } else if (std::is_same<DATA_TYPE, unsigned int>::value) {
    return gl::UnsignedInt;
  } else if (std::is_same<DATA_TYPE, float>::value) {
    return gl::Float;
  } else {
    abort();
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void Texture<DATA_TYPE, NUM_COMPONENTS>::upload(oglwrap::Texture2D& tex) const {
  upload(tex, internalFormat());
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void Texture<DATA_TYPE, NUM_COMPONENTS>::upload(oglwrap::Texture2D& tex,
                      oglwrap::PixelDataInternalFormat internal_format) const {
  using gl = oglwrap::Context;

  bool bad_alignment = (w_ * sizeof(DATA_TYPE) * NUM_COMPONENTS) % 4 != 0;
  GLint unpack_aligment;

  if(bad_alignment) {
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
    gl::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, 1);
  }

  tex.upload(
    internal_format,
    w_, h_,
    format(),
    type(),
    data().data()
  );

  if(bad_alignment) {
    gl::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, unpack_aligment);
  }
}

} // namespace engine

#endif
