// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SANDBOX_SCENE_H_
#define LOD_SANDBOX_SCENE_H_

#include "engine/scene.h"
#include "engine/gui/label.h"
#include "engine/gui/box.h"
#include "engine/gui/button.h"
#include "oglwrap/smart_enums.h"

#include "./mideu_scene.h"

struct SandboxScene : public engine::Scene {
  SandboxScene() {
    using engine::gui::Font;
    using engine::gui::Label;
    using engine::gui::Box;
    using engine::gui::Button;

    engine::gui::BoxParams params;
    params.center = glm::vec2{0.0f, 0.0f};
    params.extent = glm::vec2{1.0f, 1.0f};
    params.label_pos = glm::vec2{0.0f, 0.8f};
    params.label_text = L"Land of Dreams";
    params.label_font.set_size(80);
    params.label_font.set_color(glm::vec4{1});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.transition_height = 0.85f;
    params.bg_top_color = glm::vec4{0.3f, 0.3f, 0.3f, 0.5f};
    params.bg_top_mid_color = glm::vec4{0.6f, 0.6f, 0.6f, 0.5f};
    params.bg_bottom_mid_color = glm::vec4{0.4f, 0.4f, 0.4f, 0.5f};
    params.bg_bottom_color = glm::vec4{0.0f, 0.0f, 0.0f, 0.5f};
    addGameObject<Box>(params);

    params = engine::gui::BoxParams{};
    params.center = glm::vec2{0.0f, 0.0f};
    params.extent = glm::vec2{0.5f, 0.5f};
    params.label_pos = params.center + glm::vec2(0, 0.8f) * params.extent;
    params.label_text = L"Choose your destiny!";
    params.label_font = Font{"src/engine/gui/freetype-gl/fonts/ObelixPro.ttf", 40};
    params.label_font.set_color(glm::vec4{0, 1, 0, 0.5});
    params.label_font.set_vertical_alignment(Font::VerticalAlignment::kBottom);
    params.style = engine::gui::BoxParams::Style::kShaded;
    params.transition_height = 0.85f;
    params.roundness = 40;
    addGameObject<Box>(params);

    Font font{};
    font.set_size(35);
    font.set_color(glm::vec4{1.0, 1.0, 0.0, 0.9});
    addGameObject<Label>(L"Which pill do you take?", glm::vec2{0.0f, 0.0f}, font);

    Button *blue_pill = addGameObject<Button>(glm::vec2{-0.2f, -0.2f},
                                              glm::vec2{0.08f, 0.04f},
                                              L"dis one?",
                                              glm::vec4{1, 0, 0, 0.85},
                                              glm::vec4{1, 1, 1, 1}, 20);
      blue_pill->addPressCallback([](){engine::GameEngine::LoadScene<MideuScene>();});

    Button *orange_pill = addGameObject<Button>(glm::vec2{0.2f, -0.2f},
                                                glm::vec2{0.08f, 0.04f},
                                                L"*this one?",
                                                glm::vec4{0, 0, 1, 0.85},
                                                glm::vec4{1, 1, 1, 1}, 20);
      orange_pill->addPressCallback([](){
        std::cout << "Wrong pill, sorry. Aborting now." << std::endl;
        std::terminate();
      });
  }
};

#endif
