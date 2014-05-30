// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_LABEL_H_
#define ENGINE_GUI_LABEL_H_

#include <string>
#include "font.h"
#include "../scene.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {

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

    gl::Uniform<glm::vec2>(prog_, "uOffset") = glm::vec2(pos);

    set_text(text);

    size_.y = font.size();
  }

  glm::vec2 position() const {
    return pos_;
  }

  void set_position(glm::vec2 pos) {
    pos_ = pos;
    gl::Uniform<glm::vec2>(prog_, "uOffset") = pos;
  }

  glm::vec2 size() const {
    return size_ / screen_size_;
  }

  const std::wstring& text() {
    return text_;
  }

  void set_text(const std::wstring& text) {
    text_ = text;
    size_.x = 0;
    std::vector<glm::vec4> attribs_vec;

    font_.release();
    font_ = default_font_;
    font_.load_glyphs(text.c_str());

    for (size_t i = 0; i < text.size(); ++i) {
      texture_glyph_t *glyph = font_.get_glyph(text[i]);
      if (glyph != nullptr) {
        int kerning = 0;
        if (i > 0) {
            kerning = texture_glyph_get_kerning(glyph, text[i-1]);
        }
        size_.x += kerning;
        float x0 = size_.x + glyph->offset_x;
        float y0 = glyph->offset_y;
        float x1 = x0 + glyph->width;
        float y1 = y0 - glyph->height;
        float s0 = glyph->s0;
        float t0 = glyph->t0;
        float s1 = glyph->s1;
        float t1 = glyph->t1;

        glm::vec4 a(x0, y0, s0, t0), b(x0, y1, s0, t1);
        glm::vec4 c(x1, y0, s1, t0), d(x1, y1, s1, t1);

        attribs_vec.push_back(a);
        attribs_vec.push_back(b);
        attribs_vec.push_back(c);

        attribs_vec.push_back(d);
        attribs_vec.push_back(b);
        attribs_vec.push_back(c);

        size_.x += glyph->advance_x;
      }
    }

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

  Font& font() { return font_; }
  const Font& font() const { return font_; }
  void set_font(const Font& font) { font_ = font; }

 private:
  virtual void screenResized(size_t width, size_t height) override {
    gl::Uniform<glm::mat4>(prog_, "uProjectionMatrix") =
      glm::ortho<float>(-int(width)/2, width/2, -int(height)/2, height/2, -1, 1);
    screen_size_.x = width;
    screen_size_.y = height;
  }

  virtual void drawGui() override {
    prog_.use();
    vao_.bind();

    auto blend = gl::TemporaryEnable(gl::kBlend);
    auto cullface = gl::TemporaryDisable(gl::kCullFace);
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    gl::Texture2D::Active(0);
    font_.bindTexture();
    gl::DrawArrays(gl::kTriangles, 0, idx_cnt_);

    vao_.unbind();

    update_text();
  }

  void update_text() {
    std::wstring text = L"Amalfi kisváros (közigazgatásilag comune) Olaszország Campania régiójában, Salerno megyében. A várost valószínűleg Nagy Konstantin császár katonái alapították 320-ban. A középkorban az Amalfi Köztársaság fővárosa volt és egyben az egyik legfontosabb földközi-tengeri kikötő, Salerno vetélytársa. A nagy hajózási tapasztalatnak köszönhetően az amalfiak megalkották a Tabula Amalphitanát, a világ első hajózási törvénykönyvét, amellyel a rivális tengeri hatalmak elismerését is kivívták. Amalfi kereskedői raktárakat tartottak fenn Alexandriában, Antiokheiában és Jeruzsálemben, az utóbbi helyen 1048-ban általuk alapított Szent János-kórházból eredeztetik a johanniták rendjét. A papírgyártó műhelyeiről és a limoncellójáról világszerte híres település ma az Olaszországba látogató turisták egyik kedvenc célpontja. Itt született Flavio Gioia, a mágnestű feltalálója is. Természeti szépségei és gazdag történelmi, kulturális öröksége miatt a környező településekkel együtt 1997 óta az UNESCO Világörökségének része.";
    int len = 20;
    static int first = 0;
    std::wstring current = text.substr(++first%(text.size()-len), len);
    set_text(current);
  }

};

} // namespace engine

#endif
