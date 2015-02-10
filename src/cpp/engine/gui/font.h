// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_FONT_H_
#define ENGINE_GUI_FONT_H_

#include <string>
#include <glm/glm.hpp>
#include "freetype-gl.h"

#include "./font_manager.h"
#include "../misc.h"

namespace engine {
namespace gui {

class FontData {
  std::string filename_;
  texture_atlas_t *atlas_;
  texture_font_t *font_;
  float size_;

  void release() {
    if (font_ != nullptr) {
      if (font_->atlas != nullptr) {
        texture_atlas_delete(font_->atlas);
        font_->atlas = nullptr;
      }
      texture_font_delete(font_);
      font_ = nullptr;
    }
  }

  void load_glyphs() {
    static const wchar_t * cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                                   L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                   L"`abcdefghijklmnopqrstuvwxyz{|}~"
                                   L"áéíóöőúüűÁÉÍÓÖŐÚÜŰ";
    texture_font_load_glyphs(font_, cache);
  }

 public:
  FontData(const std::string& filename = "src/resources/fonts/Vera.ttf", float size = 12)
      : filename_(filename), atlas_(texture_atlas_new(1024, 1024, 1))
      , font_(texture_font_new_from_file(atlas_, size, filename.c_str()))
      , size_(size) {
    load_glyphs();
  }

  FontData(const FontData& f) = delete;
  FontData(FontData&& f) { *this = std::move(f);}
  FontData& operator=(const FontData& f) = delete;

  FontData& operator=(FontData&& f) {
    if (this != &f) {
      filename_ = f.filename_;
      atlas_ = f.atlas_;
      font_ = f.font_;
      size_ = f.size_;
      f.font_ = nullptr;
    }

    return *this;
  }

  ~FontData() { release(); }

  const std::string& filename() const { return filename_; }
  float size() const { return size_; }

  texture_glyph_t *get_glyph(wchar_t ch) {
    return texture_font_get_glyph(font_, ch);
  }

  void bindTexture() const {
    glBindTexture(GL_TEXTURE_2D, atlas_->id);
  }

  texture_font_t* expose() { return font_; }
};

class Font {
  // Font should be copyable, but the fontdata shouldn't be loaded for each copy
  FontData* data_;
  glm::vec4 color_;

 public:
  enum class HorizontalAlignment {kLeft, kCenter, kRight};
  enum class VerticalAlignment {kBottom, kCenter, kTop};

 private:
  HorizontalAlignment horizontal_alignment_;
  VerticalAlignment vertical_alignment_;

 public:
  Font(const std::string& filename = "src/resources/fonts/Vera.ttf",
       float size = 12,  glm::vec4 color = glm::vec4{1},
       HorizontalAlignment xalign = HorizontalAlignment::kCenter,
       VerticalAlignment yalign = VerticalAlignment::kCenter)
    : data_(FontManager::get(filename, size)), color_(color)
    , horizontal_alignment_(xalign)
    , vertical_alignment_(yalign) { }

  Font(const Font&) = default;
  Font& operator=(const Font&) = default;

  const std::string& filename() const { return data_->filename(); }
  const glm::vec4& color() const { return color_; }
  void set_color(const glm::vec4& color) { color_ = color; }
  float size() const { return data_->size(); }
  void set_size(float size) {
    data_ = FontManager::get(data_->filename(), size);
  }
  texture_font_t* expose() { return data_->expose(); }

  HorizontalAlignment horizontal_alignment() const {
    return horizontal_alignment_;
  }

  void set_horizontal_alignment(const HorizontalAlignment& align) {
    horizontal_alignment_ = align;
  }

  VerticalAlignment vertical_alignment() const { return vertical_alignment_; }

  void set_vertical_alignment(const VerticalAlignment& align) {
    vertical_alignment_ = align;
  }

  texture_glyph_t *get_glyph(wchar_t ch) {return data_->get_glyph(ch); }
  void bindTexture() const { data_->bindTexture(); }

};

} // namespace gui
} // namespace engine

#endif
