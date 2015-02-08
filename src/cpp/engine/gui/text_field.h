// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_TEXT_FIELD_H_
#define ENGINE_GUI_TEXT_FIELD_H_

#include <string>

#include "../oglwrap_config.h"
#include "../../oglwrap/smart_enums.h"

#include "./box.h"
#include "./font.h"
#include "../scene.h"
#include "../behaviour.h"

namespace engine {
namespace gui {

class TextField : public Behaviour {
 public:
  TextField(GameObject* parent, glm::vec2 center,
            glm::vec2 extent, const std::wstring& text)
      : Behaviour(parent), pos_(center), extent_(extent)
      , cursor_pos_(text.size()), text_(text), cursor_visible_(false)
      , modified_(false), focused_(false) {
    BoxParams params;
    params.center = center;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kFlat;
    params.bg_color = glm::vec4{0, 0, 0, 0.3};
    params.border_color = glm::vec4{1, 1, 1, 1};
    params.border_width = 1;
    params.roundness = 7;
    params.label_pos = center - glm::vec2(0, 0.6) * extent;
    params.label_text = text;
    params.label_cursor_pos = cursor_pos_;
    box_ = addComponent<Box>(params);
  }

 private:
  glm::vec2 pos_, extent_;
  Box *box_;
  size_t cursor_pos_;
  std::wstring text_;
  bool cursor_visible_, modified_, mouse_over_, focused_;
  float visiblity_time_;

  virtual void update() override {
    if (!focused_) {
      box_->set_text(text_, -1);
    } else {
       if (modified_) {
        visiblity_time_ = 0;
      } else {
        visiblity_time_ += scene_->game_time().dt;
      }

      if (fmod(visiblity_time_, 1.0) < 0.5) {
        if (!cursor_visible_ || modified_) {
          box_->set_text(text_, cursor_pos_);
          cursor_visible_ = true;
          modified_ = false;
        }
      } else {
        if (cursor_visible_ || modified_) {
          box_->set_text(text_, -1);
          cursor_visible_ = false;
          modified_ = false;
        }
      }
    }
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (focused_ && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      switch (key) {
        case GLFW_KEY_BACKSPACE: {
          if (cursor_pos_ > 0) {
            text_.erase(--cursor_pos_, 1);
            modified_ = true;
          }
        } break;
        case GLFW_KEY_LEFT: {
          if (cursor_pos_ > 0) {
            cursor_pos_--;
            modified_ = true;
          }
        } break;
        case GLFW_KEY_RIGHT: {
          if (cursor_pos_ < text_.size()) {
            cursor_pos_++;
            modified_ = true;
          }
        } break;
        default:
          break;
      }
    }
  }

  virtual void charTyped(unsigned codepoint) override {
    if (focused_) {
      text_.insert(cursor_pos_, 1, codepoint);
      cursor_pos_++;
      modified_ = true;
    }
  }

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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      focused_ = mouse_over_;
    }
  }
};

}  // namespace gui
}  // namespace engine

#endif
