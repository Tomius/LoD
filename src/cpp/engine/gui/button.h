// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_BUTTON_H_
#define ENGINE_GUI_BUTTON_H_

#include <vector>
#include "../behaviour.h"
#include "./box.h"

namespace engine {
namespace gui {

class Button : public Behaviour {
 public:
  Button(GameObject* parent, glm::vec2 pos, glm::vec2 extent,
         const std::wstring& text, glm::vec4 color, glm::vec4 font_color,
         int roundness = 7)
      : Behaviour(parent), pos_(pos)
      , extent_(extent), mouse_over_(false) {
    BoxParams params;
    params.center = pos;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kShaded;

    glm::vec3 top = glm::sqrt(glm::vec3(color));
    params.bg_top_color = glm::vec4{top/3.0f, color.a};
    params.bg_top_mid_color = glm::vec4{top, color.a};
    params.bg_bottom_mid_color = color;
    params.label_text = text;
    params.label_font = Font{"src/resources/fonts/VeraMoBd.ttf", 15};
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.label_font.set_color(font_color);
    params.label_pos = pos + glm::vec2(0.0f, -0.4f) * params.extent;
    params.bg_bottom_color = glm::vec4{glm::vec3(color)/3.0f, color.a};
    params.transition_height = 0.85f;
    params.border_color = glm::vec4{0, 0, 0, 1};
    params.border_width = 2;
    params.roundness = roundness;
    box_ = addComponent<Box>(params);
  }

  void addPressCallback(const std::function<void()>& func) {
    on_press_callback_.push_back(func);
  }
 private:
  glm::vec2 pos_, extent_;
  Box *box_;
  bool mouse_over_;

  std::vector<std::function<void()>> on_press_callback_;

  virtual void mouseMoved(double xpos, double ypos) override {
    glm::vec2 window_size_2 = GameEngine::window_size() / 2.0f;
    glm::vec2 ndc_pos = (glm::vec2(xpos, ypos) - window_size_2) / window_size_2;
    ndc_pos.y *= -1;

    if (pos_.x - extent_.x < ndc_pos.x && ndc_pos.x < pos_.x + extent_.x &&
       pos_.y - extent_.y < ndc_pos.y && ndc_pos.y < pos_.y + extent_.y) {
      box_->set_inverted(true);
      mouse_over_ = true;
    } else {
      box_->set_inverted(false);
      mouse_over_ = false;
    }
  }

  virtual void mouseButtonPressed(int button, int action, int mods) override {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mouse_over_) {
      for (const auto& callback : on_press_callback_) {
        callback();
      }
    }
  }
};

}  // namespace gui
}  // namespace engine

#endif
