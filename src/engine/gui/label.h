// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_LABEL_H_
#define ENGINE_GUI_LABEL_H_

#include <string>
#include <vector>

#include "../game_engine.h"
#include "../../oglwrap/smart_enums.h"

#include "./font.h"

namespace engine {
namespace gui {

class Label : public GameObject {
  Font font_;
  gl::VertexArray vao_;
  gl::ArrayBuffer attribs_;
  gl::Program prog_;

  size_t vertex_count_;
  glm::vec2 pos_, size_;
  std::wstring text_;

 public:
  Label(GameObject* parent, const std::wstring& text, glm::vec2 pos,
        const Font& font = Font{}, size_t cursor_pos = -1)
      : GameObject(parent), font_(font)
      , vertex_count_(0), pos_(pos), text_(text) {
    gl::VertexShader vs("engine/text.vert");
    gl::FragmentShader fs("engine/text.frag");

    (prog_ << vs << fs).link();
    gl::Use(prog_);
    gl::Uniform<glm::vec4>(prog_, "uColor") = font.color();

    set_text(text, cursor_pos);
    size_.y = font.size();
  }

  glm::vec2 position() const {
    return pos_;
  }

  void set_position(glm::vec2 pos) {
    pos_ = pos;

    glm::vec2 actual_pos = pos;
    if (font_.horizontal_alignment() ==
        Font::HorizontalAlignment::kCenter) {
      actual_pos.x -= size().x/2;
    } else if (font_.horizontal_alignment() ==
               Font::HorizontalAlignment::kRight) {
      actual_pos.x -= size().x;
    }

    if (font_.vertical_alignment() ==
        Font::VerticalAlignment::kCenter) {
      actual_pos.y += size().y/2;
    } else if (font_.vertical_alignment() ==
               Font::VerticalAlignment::kTop) {
      actual_pos.y += size().y;
    }

    gl::Use(prog_);
    gl::Uniform<glm::vec2>(prog_, "uOffset") = actual_pos;
  }

  glm::vec2 size() const {
    return size_ / (GameEngine::window_size()/2.0f);
  }

  const std::wstring& text() {
    return text_;
  }

  void set_text(const std::wstring& text, size_t cursor_pos = -1) {
    text_ = text;
    std::vector<glm::vec4> attribs_vec;

    float pen_x = 0, x0, x1, y0, y1, s0, t0, s1, t1;
    // We have to run to loop for one more than the text size
    // so we can draw the cursor at end of the text too
    for (size_t i = 0; i <= text_.size(); ++i) {
      if (i == cursor_pos) {  // Cursor
        // we use the black character (-1) as texture
        texture_glyph_t *glyph = font_.get_glyph(-1);

        x0 = pen_x + 1;
        y0 = font_.expose()->descender;
        x1 = pen_x + 2;
        y1 = y0 + font_.expose()->height - font_.expose()->linegap;

        s0 = glyph->s0;
        t0 = glyph->t0;
        s1 = glyph->s1;
        t1 = glyph->t1;

        glm::vec4 a(x0, y0, s0, t0), b(x0, y1, s0, t1);
        glm::vec4 c(x1, y0, s1, t0), d(x1, y1, s1, t1);

        attribs_vec.push_back(a);
        attribs_vec.push_back(b);
        attribs_vec.push_back(c);

        attribs_vec.push_back(d);
        attribs_vec.push_back(b);
        attribs_vec.push_back(c);
      }

      // The current character (glyph) in the text
      if (i < text_.size()) {
        texture_glyph_t *glyph = font_.get_glyph(text_[i]);
        if (glyph) {
          int kerning = 0;
          if (i > 0) { kerning = texture_glyph_get_kerning(glyph, text_[i-1]); }

          pen_x += kerning;
          x0 = pen_x + glyph->offset_x;
          y0 = glyph->offset_y;
          x1 = x0 + glyph->width;
          y1 = y0 - glyph->height;
          s0 = glyph->s0;
          t0 = glyph->t0;
          s1 = glyph->s1;
          t1 = glyph->t1;

          glm::vec4 a(x0, y0, s0, t0), b(x0, y1, s0, t1);
          glm::vec4 c(x1, y0, s1, t0), d(x1, y1, s1, t1);

          attribs_vec.push_back(a);
          attribs_vec.push_back(b);
          attribs_vec.push_back(c);

          attribs_vec.push_back(d);
          attribs_vec.push_back(b);
          attribs_vec.push_back(c);

          pen_x += glyph->advance_x;
        }
      }
    }

    // Update the length of the text
    size_.x = x1;

    gl::Use(prog_);
    gl::Bind(vao_);
    gl::Bind(attribs_);
    attribs_.data(attribs_vec);
    (prog_ | "aPosition").pointer(2, gl::kFloat, false,
                                  4*sizeof(GLfloat), 0).enable();
    (prog_ | "aTexCoord").pointer(2, gl::kFloat, false, 4*sizeof(GLfloat),
                                  (const void*)(2*sizeof(GLfloat))).enable();
    gl::Unbind(vao_);

    vertex_count_ = attribs_vec.size();
  }

  const Font& font() const { return font_; }
  const glm::vec4& color() const { return font_.color(); }
  void set_color(const glm::vec4& color) {
    gl::Use(prog_);
    gl::Uniform<glm::vec4>(prog_, "uColor") = color;
    font_.set_color(color);
  }
  float font_size() const { return font_.size(); }
  void set_font_size(float size) {
    font_.set_size(size);
    set_text(text_);
    size_.y = size;
  }

  Font::HorizontalAlignment horizontal_alignment() const {
    return font_.horizontal_alignment();
  }

  void set_horizontal_alignment(const Font::HorizontalAlignment& align) {
    font_.set_horizontal_alignment(align);
    set_position(pos_);
  }

  Font::VerticalAlignment vertical_alignment() const {
    return font_.vertical_alignment();
  }

  void set_vertical_alignment(const Font::VerticalAlignment& align) {
    font_.set_vertical_alignment(align);
    set_position(pos_);
  }

  virtual void screenResized(size_t width, size_t height) override {
    gl::Use(prog_);
    gl::Uniform<glm::mat4>(prog_, "uProjectionMatrix") =
      glm::ortho<float>(-int(width)/2, width/2, -int(height)/2, height/2, -1, 1);
    set_position(pos_);
  }

  virtual void render2D() override {
    gl::Use(prog_);
    gl::Bind(vao_);

    gl::ActiveTexture(0);
    font_.bindTexture();
    gl::DrawArrays(gl::kTriangles, 0, vertex_count_);

    gl::Unbind(vao_);
  }
};

}  // namespace gui
}  // namespace engine

#endif
