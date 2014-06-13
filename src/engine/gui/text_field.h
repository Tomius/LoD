// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_TEXT_FIELD_H_
#define ENGINE_GUI_TEXT_FIELD_H_

#include <string>
#include "font.h"
#include "../scene.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {
namespace gui {

class TextField {
  Box *box_;
  size_t cursor_pos_;

 public:
  TextField(glm::vec2 center, glm::vec2 extent, const std::wstring& text) {
    BoxParams params;
    params.center = pos;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kFlat;
    params.bg_color = glm::vec4{0, 0, 0, 0.5};
    params.border_color = glm::vec4{1, 1, 1, 1};
    params.border_width = 1;
    params.roundness = 7;
    box_ = addComponent<Box>(params);

  }
};

} // namespace gui
} // namespace engine

#endif
