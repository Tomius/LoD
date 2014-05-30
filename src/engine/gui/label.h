// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_LABEL_H_
#define ENGINE_GUI_LABEL_H_

#include <string>
#include "font.h"
#include "../scene.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {
namespace gui {

class Label : public engine::GameObject {
  Font font_, default_font_;
  gl::VertexArray vao_;
  gl::ArrayBuffer attribs_;
  gl::Program prog_;

  size_t idx_cnt_;
  glm::vec2 pos_, size_, screen_size_;
  std::wstring text_;

 public:
  Label(const std::wstring& text, glm::vec2 pos, const Font& font = Font{})
      : default_font_(font), idx_cnt_(0), pos_(pos), text_(text) {
    gl::VertexShader vs("text.vert");
    gl::FragmentShader fs("text.frag");

    (prog_ << vs << fs).link().use();

    gl::Uniform<glm::vec4>(prog_, "uColor") = font.color();

    set_text(text);

    size_.y = font.size();
  }

  glm::vec2 position() const {
    return pos_;
  }

  void set_position(glm::vec2 pos) {
    pos_ = pos;

    glm::vec2 actual_pos = pos;
    if (default_font_.horizontal_alignment() ==
        Font::HorizontalAlignment::kCenter) {
      actual_pos.x -= size().x/2;
    } else if (default_font_.horizontal_alignment() ==
               Font::HorizontalAlignment::kRight) {
      actual_pos.x -= size().x;
    }

    if (default_font_.vertical_alignment() ==
        Font::VerticalAlignment::kCenter) {
      actual_pos.y += size().y/2;
    } else if (default_font_.vertical_alignment() ==
               Font::VerticalAlignment::kTop) {
      actual_pos.y += size().y;
    }

    prog_.use();
    gl::Uniform<glm::vec2>(prog_, "uOffset") = actual_pos;
  }

  glm::vec2 size() const {
    return size_ / (screen_size_/2.0f);
  }

  const std::wstring& text() {
    return text_;
  }

  void set_text(const std::wstring& text) {
    text_ = text;
    std::vector<glm::vec4> attribs_vec;

    font_ = default_font_;
    font_.load_glyphs(text.c_str());

    float pen_x = 0, x0, x1, y0, y1, s0, t0, s1, t1;
    for (size_t i = 0; i < text.size(); ++i) {
      texture_glyph_t *glyph = font_.get_glyph(text[i]);
      if (glyph) {
        int kerning = 0;
        if (i > 0) { kerning = texture_glyph_get_kerning(glyph, text[i-1]); }

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

    size_.x = x1;

    prog_.use();
    vao_.bind();
    attribs_.bind();
    attribs_.data(attribs_vec);
    (prog_ | "aPosition").pointer(2, gl::kFloat, false,
                                  4*sizeof(float), 0).enable();
    (prog_ | "aTexcoord").pointer(2, gl::kFloat, false, 4*sizeof(float),
                                  (const void*)(2*sizeof(float))).enable();
    vao_.unbind();

    idx_cnt_ = attribs_vec.size();
  }

  const Font& font() const { return font_; }
  const glm::vec4& color() const { return font_.color(); }
  void set_color(const glm::vec4& color) {
    prog_.use();
    gl::Uniform<glm::vec4>(prog_, "uColor") = color;
    default_font_.set_color(color);
    set_text(text_);
  }
  float font_size() const { return default_font_.size(); }
  void set_font_size(float size) {
    default_font_.set_size(size);
    set_text(text_);
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
    prog_.use();
    gl::Uniform<glm::mat4>(prog_, "uProjectionMatrix") =
      glm::ortho<float>(-int(width)/2, width/2, -int(height)/2, height/2, -1, 1);
    screen_size_.x = width;
    screen_size_.y = height;
    set_position(pos_);
  }

  virtual void drawGui() override {
    prog_.use();
    vao_.bind();

    auto capabilities = gl::TemporarySet({{gl::kBlend, true},
                                          {gl::kCullFace, false},
                                          {gl::kDepthTest, false}});
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    gl::Texture2D::Active(0);
    font_.bindTexture();
    gl::DrawArrays(gl::kTriangles, 0, idx_cnt_);

    vao_.unbind();
  }
};

} // namespace gui
} // namespace engine

#endif
