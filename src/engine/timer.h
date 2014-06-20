// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TIMER_H_
#define ENGINE_TIMER_H_

#include "./oglwrap_config.h"
#include <GLFW/glfw3.h>

namespace engine {

class Timer {
  double last_time_;
  bool stopped_;

 public:
  double current, dt;

  Timer() : last_time_(0), stopped_(false), current(0), dt(0) {}

  double tick() {
    if (!stopped_) {
      double time = glfwGetTime();
      if (last_time_ != 0) {
        dt = time - last_time_;
        // we don't want to take really big bursts into account.
        if (dt > 0.5) {
          dt = 0;
        }
      }
      last_time_ = time;
      current += dt;
    }
    return current;
  }

  void stop() {
    stopped_ = true;
    dt = 0;
  }

  void start() {
    stopped_ = false;
    last_time_ = glfwGetTime();
  }

  void toggle() {
    if (stopped_) {
      start();
    } else {
      stop();
    }
  }
};

}  // namespace engine

#endif
