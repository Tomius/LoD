// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TEXTURE_SOURCE_INL_H_
#define ENGINE_TEXTURE_SOURCE_INL_H_

#include "texture_source.h"
#include "../oglwrap/smart_enums.h"
#include "../oglwrap/context/pixel_ops.h"

namespace engine {

template<typename DATA_TYPE, char NUM_COMPONENTS>
TextureSource<DATA_TYPE, NUM_COMPONENTS>::TextureSource(const std::string& file_name,
                                            std::string format_string) {
  // Preprocess format_string: 'S', 'C' and 'I' have special meaning
  size_t s_pos = format_string.find('S');
  if(s_pos != std::string::npos) {
    srgb_ = true;
    format_string.erase(format_string.begin() + s_pos);
    assert(NUM_COMPONENTS >= 3); // only rgb and rgba can be in srgb
  } else {
    srgb_ = false;
  }

  size_t c_pos = format_string.find('C');
  if(c_pos != std::string::npos) {
    compressed_ = true;
    format_string.erase(format_string.begin() + c_pos);
  } else {
    compressed_ = false;
  }

  size_t i_pos = format_string.find('I');
  if(i_pos != std::string::npos) {
    integer_ = true;
    format_string.erase(format_string.begin() + i_pos);
  } else {
    integer_ = false;
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
gl::PixelDataFormat TextureSource<DATA_TYPE, NUM_COMPONENTS>::format() const {
  if (integer_) {
    if (format_string_ == "R") {
      return gl::kRedInteger;
    } else if (format_string_ == "G") {
      return gl::kGreenInteger;
    } else if (format_string_ == "B") {
      return gl::kBlueInteger;
    } else if (format_string_ == "RG") {
      return gl::kRgInteger;
    } else if (format_string_ == "RGB") {
      return gl::kRgbInteger;
    } else if (format_string_ == "RGBA") {
      return gl::kRgbaInteger;
    } else if (format_string_ == "BGR") {
      return gl::kBgrInteger;
    } else if (format_string_ == "BGRA") {
      return gl::kBgraInteger;
    } else {
      abort();
    }
  } else {
    if (format_string_ == "R") {
      return gl::kRed;
    } else if (format_string_ == "G") {
      return gl::kGreen;
    } else if (format_string_ == "B") {
      return gl::kBlue;
    } else if (format_string_ == "RG") {
      return gl::kRg;
    } else if (format_string_ == "RGB") {
      return gl::kRgb;
    } else if (format_string_ == "RGBA") {
      return gl::kRgba;
    } else if (format_string_ == "BGR") {
      return gl::kBgr;
    } else if (format_string_ == "BGRA") {
      return gl::kBgra;
    } else {
      abort();
    }
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
gl::PixelDataInternalFormat
TextureSource<DATA_TYPE, NUM_COMPONENTS>::internalFormat() const {
  if(compressed_) {
    if (format_string_ == "R" || format_string_ == "G" || format_string_ == "B") {
      return gl::kCompressedRed;
    } else if (format_string_ == "RG") {
      return gl::kCompressedRg;
    } else if (format_string_ == "RGB" || format_string_ == "BGR") {
      return srgb_ ? gl::kCompressedSrgb : gl::kCompressedRgb;
    } else if (format_string_ == "RGBA" || format_string_ == "BGRA") {
      return srgb_ ? gl::kCompressedSrgbAlpha : gl::kCompressedRgba;
    } else {
      abort();
    }
  } else {
    if (format_string_ == "R" || format_string_ == "G" || format_string_ == "B") {
      return gl::kRed;
    } else if (format_string_ == "RG") {
      return gl::kRg;
    } else if (format_string_ == "RGB" || format_string_ == "BGR") {
      return srgb_ ? gl::kSrgb : gl::kRgb;
    } else if (format_string_ == "RGBA" || format_string_ == "BGRA") {
      return srgb_ ? gl::kSrgbAlpha : gl::kRgba;
    } else {
      abort();
    }
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
gl::PixelDataType TextureSource<DATA_TYPE, NUM_COMPONENTS>::type() const {
  if (std::is_same<DATA_TYPE, char>::value) {
    return gl::kByte;
  } else if (std::is_same<DATA_TYPE, unsigned char>::value) {
    return gl::kUnsignedByte;
  } else if (std::is_same<DATA_TYPE, short>::value) {
    return gl::kShort;
  } else if (std::is_same<DATA_TYPE, unsigned short>::value) {
    return gl::kUnsignedShort;
  } else if (std::is_same<DATA_TYPE, int>::value) {
    return gl::kInt;
  } else if (std::is_same<DATA_TYPE, unsigned int>::value) {
    return gl::kUnsignedInt;
  } else if (std::is_same<DATA_TYPE, float>::value) {
    return gl::kFloat;
  } else {
    abort();
  }
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void TextureSource<DATA_TYPE, NUM_COMPONENTS>::upload(gl::Texture2D& tex) const {
  upload(tex, internalFormat());
}

template<typename DATA_TYPE, char NUM_COMPONENTS>
void TextureSource<DATA_TYPE, NUM_COMPONENTS>::upload(gl::Texture2D& tex,
                      gl::PixelDataInternalFormat internal_format) const {
  bool bad_alignment = (w_ * sizeof(DATA_TYPE) * NUM_COMPONENTS) % 4 != 0;
  GLint unpack_aligment;

  if(bad_alignment) {
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
    gl::PixelStore(gl::kUnpackAlignment, 1);
  }

  tex.upload(
    internal_format,
    w_, h_,
    format(),
    type(),
    data().data()
  );

  if(bad_alignment) {
    gl::PixelStore(gl::kUnpackAlignment, unpack_aligment);
  }
}

} // namespace engine

#endif
