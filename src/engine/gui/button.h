// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_BUTTON_H_
#define ENGINE_GUI_BUTTON_H_

#include "../game_object.h"
#include "./box.h"

namespace engine {
namespace gui {

class Button : public engine::GameObject {
 public:
  Button(glm::vec2 pos, glm::vec2 extent, const std::wstring& text,
         glm::vec4 color, glm::vec4 font_color) {

    BoxParams params;
    params.center = pos;
    params.extent = extent;
    params.style = engine::gui::BoxParams::Style::kShaded;

    glm::vec3 top = glm::sqrt(glm::vec3(color));
    params.bg_top_color = glm::vec4{top/2.0f, color.a};
    params.bg_top_mid_color = glm::vec4{top, color.a};
    params.bg_bottom_mid_color = glm::vec4{glm::vec3(color)/4.0f, color.a};
    params.bg_bottom_color = color;
    params.transition_height = 0.8667f;
    params.border_color = glm::vec4{0, 0, 0, 1};
    params.border_width = 2;
    params.roundness = 7;
    addComponent<Box>(params);

    Font font("src/engine/gui/freetype-gl/fonts/VeraMoBd.ttf", 15);
    font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    font.set_color(font_color);
    addComponent<Label>(text, pos + glm::vec2(0.0f, -0.4f) * params.extent, font);
  }
};

} // namespace gui
} // namespace engine

#endif
