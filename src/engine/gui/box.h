// Copyright (c) 2014, Tamas Csala

#include "label.h"
#include "../../oglwrap/shapes/full_screen_rect.h"

namespace engine {
namespace gui {

struct BoxParams {
  glm::vec2 center, extent;
  std::wstring label_text = L"";
  Font label_font = Font{};
  glm::vec2 label_pos;
  enum class Style {kFlat, kShaded} style = Style::kFlat;
  glm::vec4 bg_color = glm::vec4{0.5f};
  glm::vec4 bg_top_color = glm::vec4{0.3f, 0.3f, 0.3f, 1};
  glm::vec4 bg_top_mid_color = glm::vec4{0.6f, 0.6f, 0.6f, 1};
  glm::vec4 bg_bottom_mid_color = glm::vec4{0.04f, 0.1f, 0.2f, 1};
  glm::vec4 bg_bottom_color = glm::vec4{0.01f, 0.025f, 0.05f, 1};
  float transition_height = 0.75f;
  glm::vec4 border_color = glm::vec4{1.0f};
  float border_width = 1;
};

class Box : public engine::GameObject {
  BoxParams params_;

  gl::FullScreenRectangle rect_;
  gl::Program prog_;

 public:
  Box(const BoxParams& params) : params_(params) {
    gl::VertexShader vs("box.vert");
    gl::FragmentShader fs("box.frag");
    prog_ << vs << fs;
    prog_.link().use();

    rect_.setupPositions(prog_ | "aPosition");
    rect_.setupTexCoords(prog_ | "aTexCoord");
    gl::Uniform<glm::vec2>(prog_, "uOffset") = params_.center;
    gl::Uniform<glm::vec2>(prog_, "uScale") = params_.extent;
    gl::Uniform<glm::vec4>(prog_, "uBorderColor") = params_.border_color;
    gl::Uniform<float>(prog_, "uBorderPixels") = params_.border_width;

    if(params_.style == BoxParams::Style::kShaded) {
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
      addComponent<Label>(params_.label_text, params_.label_pos, params_.label_font);
    }
  }

  virtual void screenResized(const Scene&, size_t width, size_t height) override {
    glm::vec2 border_width =
      params_.border_width / (params_.extent * glm::vec2(width, height));

    prog_.use();
    gl::Uniform<glm::vec2>(prog_, "uBorderWidth") = border_width;

    glm::vec2 corners[4] = {
      glm::vec2{-1, -1}, glm::vec2{-1, +1}, glm::vec2{+1, -1}, glm::vec2{+1, +1}
    };

    for (int i = 0; i < 4; ++i) {
      glm::vec2 corner = params_.extent * corners[i] + params_.center;
      corner = (1.0f + corner) * 0.5f; // [-1, 1] -> [0, 1]
      corner *= glm::vec2(width, height);

      // offset it with 10 px towards the center
      corner -= corners[i] * glm::vec2(10);

      // and upload the uniform
      std::string name = "uCorners[" + std::to_string(i) + "]";
      gl::Uniform<glm::vec2>(prog_, name) = corner;
    }
  }

  virtual void render2D(const Scene& scene) override {
    prog_.use();
    rect_.render();
  }
};

}
}
