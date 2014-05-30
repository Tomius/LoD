// Copyright (c) 2014, Tamas Csala

#ifndef LOD_FPS_DISPLAY_H_
#define LOD_FPS_DISPLAY_H_

#include "engine/game_object.h"
#include "engine/gui/label.h"
#include "engine/gui/font.h"

class FpsDisplay : public engine::GameObject {
  engine::gui::Font font_;
  engine::gui::Label label_;

  static constexpr float refresh_interval = 0.1f;

  virtual void screenResized(size_t width, size_t height) override {
    label_.screenResized(width, height);
  }

  virtual void update(float time) {
    static double accum_time = 0.0f;
    static double last_call = time;
    double dt = time - last_call;
    last_call = time;
    static int calls = 0;

    calls++;
    accum_time += dt;
    if (accum_time > refresh_interval) {
      label_.set_text(L"FPS: " +
        std::to_wstring(int(calls * (1.0f/accum_time))));
      accum_time = calls = 0;
    }
  }

  virtual void drawGui() override {
    label_.drawGui();
  }

 public:
  FpsDisplay() : font_("src/engine/gui/freetype-gl/fonts/Vera.ttf", 30,
                       glm::vec4(1, 0, 0, 1),
                       engine::gui::Font::HorizontalAlignment::kCenter,
                       engine::gui::Font::VerticalAlignment::kCenter)
               , label_(L"FPS: ", glm::vec2{0.8f, 0.9f}, font_) {}

};

#endif
