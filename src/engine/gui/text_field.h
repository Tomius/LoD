// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_TEXT_FIELD_H_
#define ENGINE_GUI_TEXT_FIELD_H_

#include <string>

#include "../../lod_oglwrap_config.h"
#include "../../oglwrap/smart_enums.h"

#include "./box.h"
#include "./font.h"
#include "../scene.h"
#include "../behaviour.h"

namespace engine {
namespace gui {

class TextField : public engine::Behaviour {
  Box *box_;
  size_t cursor_pos_;
  std::wstring text_;
  bool cursor_visible_, modified_;
  float visiblity_time_;

  virtual void update(const Scene& scene) override {
    if (modified_) {
      visiblity_time_ = 0;
    } else {
      visiblity_time_ += scene.game_time().dt;
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

  virtual void keyAction(const Scene& scene, int key, int scancode,
                         int action, int mods) override {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
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

  virtual void charTyped(const Scene& scene, unsigned codepoint) override {
    text_.insert(cursor_pos_, 1, codepoint);
    cursor_pos_++;
    modified_ = true;
  }

 public:
  TextField(glm::vec2 center, glm::vec2 extent, const std::wstring& text)
      : cursor_pos_(text.size()), text_(text)
      , cursor_visible_(false), modified_(false) {
    BoxParams params;
    params.center = center;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kFlat;
    params.bg_color = glm::vec4{0, 0, 0, 0.3};
    params.border_color = glm::vec4{1, 1, 1, 1};
    params.border_width = 1;
    params.roundness = 7;
    params.label_pos = center - glm::vec2(0, 0.5) * extent;
    params.label_text = text;
    params.label_cursor_pos = cursor_pos_;
    box_ = addComponent<Box>(params);
  }
};

}  // namespace gui
}  // namespace engine

#endif
