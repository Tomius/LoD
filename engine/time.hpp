#ifndef ENGINE_TIME_HPP_
#define ENGINE_TIME_HPP_

#include <SFML/Window.hpp>

namespace engine {
  class Timer {
    sf::Clock clock_;
    float last_time_ = 0;
    bool stopped_ = false;
  public:

    float current = 0, dt = 0;

    float tick() {
      if(!stopped_) {
        float time = clock_.getElapsedTime().asSeconds();
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
      last_time_ = clock_.getElapsedTime().asSeconds();
    }

    void toggle() {
      if(stopped_) {
        start();
      } else {
        stop();
      }
    }
  };
}


#endif
