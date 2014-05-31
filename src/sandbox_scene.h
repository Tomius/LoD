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
    font.set_size(30);
    font.set_color(glm::vec4{0.0, 0.7, 1.0, 0.9});
    addGameObject<Label>(L"This is a random blue label", glm::vec2{0.0f, 0.0f}, font);

    font.set_size(25);
    font.set_color(glm::vec4{1});
    addGameObject<Label>(L"This scene is for testing GUI elements.",
                         glm::vec2{0.0f, 0.9f}, font);
    addGameObject<Label>(
      L"Please comment out a line in 'src/main.cc' if you want to see the real Land of Dreams scene.",
      glm::vec2{0.0f, 0.8f}, font);

    font.set_size(15);
    font.set_color(glm::vec4{0, 1, 0, 1});
    addGameObject<Label>(L"This is the title of the box", glm::vec2{0.0f, 0.4f}, font);
    addGameObject<Box>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.5f, 0.5f});
  }
};

#endif
