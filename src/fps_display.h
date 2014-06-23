// Copyright (c) 2014, Tamas Csala

#ifndef LOD_FPS_DISPLAY_H_
#define LOD_FPS_DISPLAY_H_

#include "engine/scene.h"
#include "engine/behaviour.h"
#include "engine/gui/label.h"

class FpsDisplay : public engine::Behaviour {
 public:
  explicit FpsDisplay(engine::Scene* scene) : engine::Behaviour(scene) {
    label_ = addComponent<engine::gui::Label>(L"FPS: ", glm::vec2{0.8f, 0.9f},
             engine::gui::Font{"src/engine/gui/freetype-gl/fonts/Vera.ttf", 30,
             glm::vec4(1, 0, 0, 1)});
  }

 private:
  engine::gui::Label *label_;
  static constexpr float refresh_interval = 0.1f;

  virtual void update() override {
    static double accum_time = 0;
    static int calls = 0;

    calls++;
    accum_time += scene_->camera_time().dt;
    if (accum_time > refresh_interval) {
      label_->set_text(L"FPS: " +
        std::to_wstring(static_cast<int>(calls * (1.0f/accum_time))));
      accum_time = calls = 0;
    }
  }
};

#endif
