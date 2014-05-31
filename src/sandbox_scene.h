// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SANDBOX_SCENE_H_
#define LOD_SANDBOX_SCENE_H_

#include <wchar.h>
#include "engine/scene.h"
#include "engine/gui/label.h"
#include "engine/gui/box.h"
#include "oglwrap/smart_enums.h"

struct SandboxScene : public engine::Scene {
  SandboxScene() {
    using engine::gui::Font;
    using engine::gui::Label;
    using engine::gui::Box;

    engine::gui::BoxParams params;
    params.center = glm::vec2{0.0f, 0.0f};
    params.extent = glm::vec2{1.0f, 1.0f};
    params.label_pos = glm::vec2{0.0f, 0.9f};
    params.label_text = L"This scene is for testing GUI elements.";
    params.label_font.set_size(25);
    params.label_font.set_color(glm::vec4{1});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.transition_height = 0.85f;
    params.bg_top_color = glm::vec4{0.3f, 0.3f, 0.3f, 0.5f};
    params.bg_top_mid_color = glm::vec4{0.6f, 0.6f, 0.6f, 0.5f};
    params.bg_bottom_mid_color = glm::vec4{0.2f, 0.2f, 0.2f, 0.5f};
    params.bg_bottom_color = glm::vec4{0.1f, 0.1f, 0.1f, 0.5f};
    addGameObject<Box>(params);

    Font font;
    font.set_size(25);
    font.set_color(glm::vec4{1});
    addGameObject<Label>(
      L"Please comment out a line in 'src/main.cc' if you want to see the real Land of Dreams scene.",
      glm::vec2{0.0f, 0.8f}, font);

    params = engine::gui::BoxParams{};
    params.center = glm::vec2{0.0f, 0.0f};
    params.extent = glm::vec2{0.5f, 0.5f};
    params.label_pos = params.center + glm::vec2(0, 0.8f) * params.extent;
    params.label_text = L"I'm the title of this box";
    params.label_font.set_size(40);
    params.label_font.set_color(glm::vec4{0, 1, 0, 0.5});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.transition_height = 0.85f;
    addGameObject<Box>(params);

    font.set_size(30);
    font.set_color(glm::vec4{1.0, 1.0, 0.0, 0.9});
    addGameObject<Label>(L"Which pill would you take?", glm::vec2{0.0f, 0.0f}, font);

    params = engine::gui::BoxParams{};
    params.center = glm::vec2{-0.2f, -0.2f};
    params.extent = glm::vec2{0.1f, 0.05f};
    params.label_text = L"Dis one?";
    params.label_font.set_size(15);
    params.label_font.set_color(glm::vec4{0, 0, 0, 1});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.label_pos = params.center + glm::vec2(0, -0.2) * params.extent;
    params.bg_color = glm::vec4(0, 0.7, 1, 1);
    addGameObject<Box>(params);

    params.center = glm::vec2{0.2f, -0.2f};
    params.extent = glm::vec2{0.1f, 0.05f};
    params.label_text = L"Or *this one?";
    params.label_font.set_size(15);
    params.label_font.set_color(glm::vec4{0, 0, 0, 1});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.label_pos = params.center + glm::vec2(0, -0.2) * params.extent;
    params.bg_color = glm::vec4(1, 0.5, 0, 1);
    addGameObject<Box>(params);
  }
};

#endif
