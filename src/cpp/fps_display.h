// Copyright (c) 2014, Tamas Csala

#ifndef LOD_FPS_DISPLAY_H_
#define LOD_FPS_DISPLAY_H_

#include "engine/scene.h"
#include "engine/behaviour.h"
#include "engine/gui/label.h"

class FpsDisplay : public engine::Behaviour {
 public:
  explicit FpsDisplay(engine::GameObject* parent)
      : engine::Behaviour(parent), kRefreshInterval(0.1f)
      , sum_frame_num_(0), sum_time_(0) {
    label_ = addComponent<engine::gui::Label>(L"FPS: ", glm::vec2{0.8f, 0.9f},
             engine::gui::Font{"src/resources/fonts/Vera.ttf", 30,
             glm::vec4(1, 0, 0, 1)});
  }

  ~FpsDisplay() {
    std::cout << "Average FPS: " << sum_frame_num_ / sum_time_ << std::endl;
  }

 private:
  engine::gui::Label *label_;
  const float kRefreshInterval;
  double sum_frame_num_, sum_time_;

  virtual void update() override {
    static double accum_time = scene_->camera_time().dt;
    static int calls = 0;

    calls++;
    accum_time += scene_->camera_time().dt;
    if (accum_time > kRefreshInterval) {
      label_->set_text(L"FPS: " +
        std::to_wstring(static_cast<int>(calls / accum_time)));
      sum_frame_num_ += calls;
      sum_time_ += accum_time;
      accum_time = calls = 0;
    }
  }
};

#endif
