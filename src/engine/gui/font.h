// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_FONT_H_
#define ENGINE_GUI_FONT_H_

#include <string>
#include "freetype-gl/freetype-gl.h"
#include "../../oglwrap/glm/glm/glm.hpp"

namespace engine {

class Font {
  std::string filename_;
  texture_atlas_t *atlas_;
  texture_font_t *font_;
  glm::vec4 color_;
  float size_;
 public:
  Font(const std::string& filename = "src/engine/gui/freetype-gl/fonts/Vera.ttf",
       float size = 12,  glm::vec4 color = glm::vec4{0,0,0,1})
    : filename_(filename), atlas_(texture_atlas_new(1024, 1024, 1))
    , font_(texture_font_new_from_file(atlas_, size, filename.c_str()))
    , color_(color), size_(size) { }

  Font(const Font& f) { font_ = nullptr; *this = f; }
  Font(Font&& f) { *this = std::move(f);}

  Font& operator=(const Font& f) {
    if (this != &f) {
      release();
      *this = Font{f.filename_, f.size_, f.color_};
    }

    return *this;
  }

  Font& operator=(Font&& f) {
    if (this != &f) {
      filename_ = f.filename_;
      atlas_ = f.atlas_;
      font_ = f.font_;
      color_ = f.color_;
      size_ = f.size_;
      f.font_ = nullptr;
    }

    return *this;
  }

  ~Font() {
    release();
  }

  void release() {
    if (font_ != nullptr) {
      if(font_->atlas != nullptr) {
        texture_atlas_delete(font_->atlas);
        font_->atlas = nullptr;
      }
      texture_font_delete(font_);
      font_ = nullptr;
    }
  }

  const std::string& filename() const { return filename_; }
  const glm::vec4& color() const { return color_; }
  void set_color(const glm::vec4& color) { color_ = color; }
  float size() const { return size_; }
  void set_size(float size) { size_ = size; }

  texture_glyph_t *get_glyph(wchar_t ch) {
    return texture_font_get_glyph(font_, ch);
  }

  void load_glyphs(const wchar_t* text) {
    texture_font_load_glyphs(font_, text);
  }

  void bindTexture() {
    glBindTexture(GL_TEXTURE_2D, atlas_->id);
  }

};

} // namespace engine

#endif
