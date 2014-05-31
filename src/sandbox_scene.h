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

    Font font;
    font.set_size(40);
    font.set_color(glm::vec4{0, 1, 0, 0.5});
    font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    engine::gui::BoxParams params;
    params.center = glm::vec2{0.0f, 0.0f};
    params.extent = glm::vec2{0.5f, 0.5f};
    params.label_pos = params.center + glm::vec2(0, 0.8f) * params.extent;
    params.label_text = L"I'm the title of this box";
    params.label_font = font;
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.transition_height = 0.85f;
    addGameObject<Box>(params);

    font.set_size(30);
    font.set_color(glm::vec4{0.0, 0.7, 1.0, 0.9});
    addGameObject<Label>(L"This is a random blue label", glm::vec2{0.0f, 0.0f}, font);

    font.set_size(15);
    font.set_color(glm::vec4{1.0, 1.0, 0.0, 1.0});
    addGameObject<Label>(L"- No, this is Patrick!", glm::vec2{0.0f, -0.05f}, font);

    font.set_size(25);
    font.set_color(glm::vec4{1});
    addGameObject<Label>(L"This scene is for testing GUI elements.",
                         glm::vec2{0.0f, 0.9f}, font);
    addGameObject<Label>(
      L"Please comment out a line in 'src/main.cc' if you want to see the real Land of Dreams scene.",
      glm::vec2{0.0f, 0.8f}, font);

    font.set_size(15);
    font.set_color(glm::vec4{0, 1, 1, 1});
    font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.center = glm::vec2{0.3f, -0.2f};
    params.extent = glm::vec2{0.1f, 0.05f};
    params.label_text = L"This is a smaller box";
    params.label_font = font;
    params.label_pos = params.center + glm::vec2(0, -0.2) * params.extent;
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.bg_top_color = glm::vec4{0.3f, 0.4f, 0.3f, 1};
    params.bg_top_mid_color = glm::vec4{0.6f, 0.8f, 0.6f, 1};
    params.bg_bottom_mid_color = glm::vec4{0.04f, 0.2f, 0.1f, 1};
    params.bg_bottom_color = glm::vec4{0.01f, 0.05f, 0.025f, 1};
    params.transition_height = 0.75f;
    addGameObject<Box>(params);
  }
};

#endif
