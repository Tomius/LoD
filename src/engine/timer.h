// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_TIMER_H_
#define ENGINE_TIMER_H_

#include <GLFW/glfw3.h>

namespace engine {

class Timer {
  double last_time_ = 0;
  bool stopped_ = false;
 public:
  double current = 0, dt = 0;

  double tick() {
    if (!stopped_) {
      double time = glfwGetTime();
      dt = time - last_time_;
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

}


#endif
