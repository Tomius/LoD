// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_BUTTON_H_
#define ENGINE_GUI_BUTTON_H_

#include "../game_object.h"
#include "./box.h"

namespace engine {
namespace gui {

class Button : public engine::GameObject {
  glm::vec2 pos_, extent_;
  Box *box_;

  virtual void mouseMoved(const Scene& scene, double xpos, double ypos) override {
    glm::vec2 window_size_2 = GameEngine::window_size() / 2.0f;
    glm::vec2 ndc_pos = (glm::vec2(xpos, ypos) - window_size_2) / window_size_2;
    ndc_pos.y *= -1;

    if(pos_.x - extent_.x < ndc_pos.x && ndc_pos.x < pos_.x + extent_.x &&
       pos_.y - extent_.y < ndc_pos.y && ndc_pos.y < pos_.y + extent_.y) {
      box_->set_inverted(true);
    } else {
      box_->set_inverted(false);
    }
  }

 public:
  Button(glm::vec2 pos, glm::vec2 extent, const std::wstring& text,
         glm::vec4 color, glm::vec4 font_color) : pos_(pos), extent_(extent) {

    BoxParams params;
    params.center = pos;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kShaded;

    glm::vec3 top = glm::sqrt(glm::vec3(color));
    params.bg_top_color = glm::vec4{top/3.0f, color.a};
    params.bg_top_mid_color = glm::vec4{top, color.a};
    params.bg_bottom_mid_color = color;
    params.bg_bottom_color = glm::vec4{glm::vec3(color)/3.0f, color.a};
    params.transition_height = 0.85f;
    params.border_color = glm::vec4{0, 0, 0, 1};
    params.border_width = 2;
    params.roundness = 7;
    box_ = addComponent<Box>(params);

    Font font("src/engine/gui/freetype-gl/fonts/VeraMoBd.ttf", 15);
    font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    font.set_color(font_color);
    addComponent<Label>(text, pos + glm::vec2(0.0f, -0.4f) * params.extent, font);
  }
};

} // namespace gui
} // namespace engine

#endif
