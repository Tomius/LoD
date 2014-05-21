// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TEXTURE_INL_H_
#define ENGINE_TEXTURE_INL_H_

#include "texture.h"

namespace engine {

template<typename DATA_TYPE, char NUM_COMPONENTS>
Texture<DATA_TYPE, NUM_COMPONENTS>::Texture(const std::string& file_name,
                 const std::string& format_string, bool integer)
    : integer_(integer), format_string_(format_string) {

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
  using oglwrap::PixelDataFormat;

  if (integer_) {
    if (format_string_ == "R") {
      return PixelDataFormat::Red_Integer;
    } else if (format_string_ == "G") {
      return PixelDataFormat::Green_Integer;
    } else if (format_string_ == "B") {
      return PixelDataFormat::Blue_Integer;
    } else if (format_string_ == "RG") {
      return PixelDataFormat::RG_Integer;
    } else if (format_string_ == "RGB") {
      return PixelDataFormat::RGB_Integer;
    } else if (format_string_ == "RGBA") {
      return PixelDataFormat::RGBA_Integer;
    } else if (format_string_ == "BGR") {
      return PixelDataFormat::BGR_Integer;
    } else if (format_string_ == "BGRA") {
      return PixelDataFormat::BGRA_Integer;
    } else {
      abort();
    }
  } else {
    if (format_string_ == "R") {
      return PixelDataFormat::Red;
    } else if (format_string_ == "G") {
      return PixelDataFormat::Green;
    } else if (format_string_ == "B") {
      return PixelDataFormat::Blue;
    } else if (format_string_ == "RG") {
      return PixelDataFormat::RG;
    } else if (format_string_ == "RGB") {
      return PixelDataFormat::RGB;
    } else if (format_string_ == "RGBA") {
      return PixelDataFormat::RGBA;
    } else if (format_string_ == "BGR") {
      return PixelDataFormat::BGR;
    } else if (format_string_ == "BGRA") {
      return PixelDataFormat::BGRA;
    } else {
      abort();
    }
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
oglwrap::PixelDataInternalFormat
Texture<DATA_TYPE, NUM_COMPONENTS>::internalFormat() const {
  using oglwrap::PixelDataInternalFormat;

  if (format_string_ == "R" || format_string_ == "G" || format_string_ == "B") {
    return PixelDataInternalFormat::Red;
  } else if (format_string_ == "RG") {
    return PixelDataInternalFormat::RG;
  } else if (format_string_ == "RGB" || format_string_ == "BGR") {
    return PixelDataInternalFormat::RGB;
  } else if (format_string_ == "RGBA" || format_string_ == "BGRA") {
    return PixelDataInternalFormat::RGBA;
  } else {
    abort();
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
oglwrap::PixelDataType Texture<DATA_TYPE, NUM_COMPONENTS>::type() const {
  using oglwrap::PixelDataType;

  if (std::is_same<DATA_TYPE, char>::value) {
    return PixelDataType::Byte;
  } else if (std::is_same<DATA_TYPE, unsigned char>::value) {
    return PixelDataType::UnsignedByte;
  } else if (std::is_same<DATA_TYPE, short>::value) {
    return PixelDataType::Short;
  } else if (std::is_same<DATA_TYPE, unsigned short>::value) {
    return PixelDataType::UnsignedShort;
  } else if (std::is_same<DATA_TYPE, int>::value) {
    return PixelDataType::Int;
  } else if (std::is_same<DATA_TYPE, unsigned int>::value) {
    return PixelDataType::UnsignedInt;
  } else if (std::is_same<DATA_TYPE, float>::value) {
    return PixelDataType::Float;
  } else {
    abort();
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void Texture<DATA_TYPE, NUM_COMPONENTS>::upload(oglwrap::Texture2D& tex) const {
  using oglwrap::Context;
  GLint unpack_aligment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
  Context::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, 1);
  tex.upload(
    internalFormat(),
    w_, h_,
    format(),
    type(),
    data().data()
  );
  Context::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, unpack_aligment);
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void Texture<DATA_TYPE, NUM_COMPONENTS>::upload(oglwrap::Texture2D& tex,
                      oglwrap::PixelDataInternalFormat internal_format) const {
  using oglwrap::Context;
  GLint unpack_aligment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
  Context::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, 1);
  tex.upload(
    internal_format,
    w_, h_,
    format(),
    type(),
    data().data()
  );
  Context::PixelStore(oglwrap::PixelStorageMode::UnpackAlignment, unpack_aligment);
}

} // namespace engine

#endif
