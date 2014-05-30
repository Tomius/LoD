// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SANDBOX_SCENE_H_
#define LOD_SANDBOX_SCENE_H_

#include <wchar.h>
#include "engine/scene.h"
#include "engine/gui/label.h"
#include "oglwrap/smart_enums.h"

struct SandboxScene : public engine::Scene {
  SandboxScene() {
    using engine::gui::Font;

    Font font;
    font.set_size(30);
    font.set_color(glm::vec4{0.0, 0.7, 1.0, 0.9});
    font.set_vertical_alignment(Font::VerticalAlignment::kCenter);
    font.set_horizontal_alignment(Font::HorizontalAlignment::kCenter);

    addGameObject<engine::gui::Label>(L"Hello world!", glm::vec2{0.0f, 0.0f}, font);
  }
};

#endif
