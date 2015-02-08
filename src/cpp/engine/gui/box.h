// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_BOX_H_
#define ENGINE_GUI_BOX_H_

#include <string>
#include "./label.h"
#include "../../oglwrap/shapes/rectangle_shape.h"

namespace engine {
namespace gui {

struct BoxParams {
  glm::vec2 center, extent;
  std::wstring label_text = L"";
  Font label_font = Font{};
  glm::vec2 label_pos;
  size_t label_cursor_pos = -1;
  enum class Style {kFlat, kShaded} style = Style::kFlat;
  glm::vec4 bg_color = glm::vec4{0.5f};
  glm::vec4 bg_top_color = glm::vec4{0.3f, 0.3f, 0.3f, 1};
  glm::vec4 bg_top_mid_color = glm::vec4{0.6f, 0.6f, 0.6f, 1};
  glm::vec4 bg_bottom_mid_color = glm::vec4{0.04f, 0.1f, 0.2f, 1};
  glm::vec4 bg_bottom_color = glm::vec4{0.01f, 0.025f, 0.05f, 1};
  float transition_height = 0.75f;
  glm::vec4 border_color = glm::vec4{1.0f};
  float border_width = 1;
  float roundness = 10;
};

class Box : public engine::GameObject {
 public:
  Box(GameObject* parent, const BoxParams& params)
      : engine::GameObject(parent), params_(params), label_(nullptr)
      , rect_({gl::RectangleShape::kPosition, gl::RectangleShape::kTexCoord}) {
    gl::VertexShader vs("engine/box.vert");
    gl::FragmentShader fs("engine/box.frag");
    (prog_ << vs << fs).link();
    gl::Use(prog_);

    (prog_ | "aPosition").bindLocation(rect_.kPosition);
    (prog_ | "aTexCoord").bindLocation(rect_.kTexCoord);
    gl::Uniform<glm::vec2>(prog_, "uOffset") = params_.center;
    gl::Uniform<glm::vec2>(prog_, "uScale") = params_.extent;
    gl::Uniform<glm::vec4>(prog_, "uBorderColor") = params_.border_color;
    gl::Uniform<float>(prog_, "uBorderPixels") = params_.border_width;
    gl::Uniform<float>(prog_, "uRoundness") = params_.roundness;

    if (params_.style == BoxParams::Style::kShaded) {
      gl::Uniform<glm::vec4>(prog_, "uBgTopColor") = params_.bg_top_color;
      gl::Uniform<glm::vec4>(prog_, "uBgTopMidColor") = params_.bg_top_mid_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomMidColor") = params_.bg_bottom_mid_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomColor") = params_.bg_bottom_color;
      gl::Uniform<float>(prog_, "uTransitionHeight") = params_.transition_height;
    } else {
      gl::Uniform<glm::vec4>(prog_, "uBgColor") = params_.bg_color;
      gl::Uniform<float>(prog_, "uTransitionHeight") = -1.0f;
    }

    if(!params_.label_text.empty()) {
      label_ = addComponent<Label>(params_.label_text, params_.label_pos,
                                   params_.label_font, params_.label_cursor_pos);
    }
  }

  void set_inverted(bool value) {
    gl::Use(prog_);
    if (value) {
      gl::Uniform<glm::vec4>(prog_, "uBgTopColor") = params_.bg_top_mid_color;
      gl::Uniform<glm::vec4>(prog_, "uBgTopMidColor") = params_.bg_top_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomMidColor") = params_.bg_bottom_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomColor") = params_.bg_bottom_mid_color;
    } else {
      gl::Uniform<glm::vec4>(prog_, "uBgTopColor") = params_.bg_top_color;
      gl::Uniform<glm::vec4>(prog_, "uBgTopMidColor") = params_.bg_top_mid_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomMidColor") = params_.bg_bottom_mid_color;
      gl::Uniform<glm::vec4>(prog_, "uBgBottomColor") = params_.bg_bottom_color;
    }
  }

  std::wstring text() const {
    return label_ ? label_->text() : L"";
  }

  void set_text(const std::wstring& text, size_t cursor_pos = -1) {
    if (label_) {
      label_->set_text(text, cursor_pos);
    } else {
      label_ = addComponent<Label>(text, params_.label_pos,
                                   params_.label_font, cursor_pos);
    }
  }

 private:
  BoxParams params_;
  Label *label_;

  gl::RectangleShape rect_;
  gl::Program prog_;

  virtual void screenResized(size_t width, size_t height) override {
    glm::vec2 border_width = params_.border_width /
        (params_.extent * glm::vec2(0.99f * width, 0.99f * height));

    gl::Use(prog_);
    gl::Uniform<glm::vec2>(prog_, "uBorderWidth") = border_width;

    glm::vec2 corners[4] = {glm::vec2{-1, -1}, glm::vec2{-1, +1},
                            glm::vec2{+1, -1}, glm::vec2{+1, +1}};

    for (int i = 0; i < 4; ++i) {
      glm::vec2 corner = params_.extent * corners[i] + params_.center;
      corner = (1.0f + corner) * 0.5f;  // [-1, 1] -> [0, 1]
      corner *= glm::vec2(width, height);

      // offset it with 'roundness' px towards the center
      corner -= corners[i] * glm::vec2(params_.roundness);

      // and upload the uniform
      std::string name = "uCorners[" + std::to_string(i) + "]";
      gl::Uniform<glm::vec2>(prog_, name) = corner;
    }
  }

  virtual void render2D() override {
    gl::Use(prog_);
    rect_.render();
  }
};

}  // namespace gui
}  // namespace engine

#endif
