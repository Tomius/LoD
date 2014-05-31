// Copyright (c) 2014, Tamas Csala

#include "label.h"
#include "../../oglwrap/shapes/full_screen_rect.h"

namespace engine {
namespace gui {

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }
glm::vec2 sgn(glm::vec2 val) { return glm::vec2(sgn(val.x), sgn(val.y));}

class Box : public engine::GameObject {
  glm::vec2 center_, extent_;
  glm::vec4 bg_color_, border_color_;

  gl::FullScreenRectangle rect_;
  gl::Program prog_;
  float border_width_in_pixels_;

 public:
  Box(glm::vec2 center, glm::vec2 extent, const std::wstring& text = L"",
      glm::vec4 bg_color = glm::vec4{0.5f}, glm::vec4 border_color = glm::vec4{1.0f},
      float border_width = 1)
      : center_(center), extent_(extent)
      , bg_color_(bg_color), border_color_(border_color)
      , border_width_in_pixels_(border_width) {
    gl::VertexShader vs("box.vert");
    gl::FragmentShader fs("box.frag");
    prog_ << vs << fs;
    prog_.link().use();

    rect_.setupPositions(prog_ | "aPosition");
    rect_.setupTexCoords(prog_ | "aTexCoord");
    gl::Uniform<glm::vec2>(prog_, "uOffset") = center;
    gl::Uniform<glm::vec2>(prog_, "uScale") = extent;
    gl::Uniform<glm::vec4>(prog_, "uBgColor") = bg_color;
    gl::Uniform<glm::vec4>(prog_, "uBorderColor") = border_color;
    gl::Uniform<float>(prog_, "uBorderPixels") = border_width;
  }

  virtual void screenResized(size_t width, size_t height) override {
    glm::vec2 border_width =
      border_width_in_pixels_ / (extent_ * glm::vec2(width, height));

    prog_.use();
    gl::Uniform<glm::vec2>(prog_, "uBorderWidth") = border_width;

    glm::vec2 corners[4] = {
      glm::vec2{-1, -1}, glm::vec2{-1, +1}, glm::vec2{+1, -1}, glm::vec2{+1, +1}
    };

    for (int i = 0; i < 4; ++i) {
      glm::vec2 corner = extent_ * corners[i] + center_;
      corner = (1.0f + corner) * 0.5f; // [-1, 1] -> [0, 1]
      corner *= glm::vec2(width, height);

      // offset it with 10 px towards the center
      corner -= corners[i] * glm::vec2(10);

      // and upload the uniform
      std::string name = "uCorners[" + std::to_string(i) + "]";
      gl::Uniform<glm::vec2>(prog_, name) = corner;
    }
  }

  virtual void drawGui() override {
    prog_.use();
    rect_.render();
  }
};

}
}
