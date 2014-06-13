// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_BEHAVIOUR_H_
#define ENGINE_BEHAVIOUR_H_

#include "./game_object.h"

namespace engine {

class Behaviour : public GameObject {
 public:
  virtual void update(const Scene& scene) {}
  virtual void keyAction(const Scene& scene, int key, int scancode,
                         int action, int mods) {}
  virtual void mouseScrolled(const Scene& scene, double xoffset,
                             double yoffset) {}
  virtual void mouseButtonPressed(const Scene& scene, int button,
                                  int action, int mods) {}
  virtual void mouseMoved(const Scene& scene, double xpos, double ypos) {}

  virtual void updateAll(const Scene& scene) override {
    for (auto& component : behaviours_) {
      component->updateAll(scene);
    }
    update(scene);
  }

  virtual void keyActionAll(const Scene& scene, int key, int scancode,
                            int action, int mods) override {
    for (auto& component : behaviours_) {
      component->keyActionAll(scene, key, scancode, action, mods);
    }
    keyAction(scene, key, scancode, action, mods);
  }

  virtual void mouseScrolledAll(const Scene& scene, double xoffset,
                                double yoffset) override {
    for (auto& component : behaviours_) {
      component->mouseScrolledAll(scene, xoffset, yoffset);
    }
    mouseScrolled(scene, xoffset, yoffset);
  }

  virtual void mouseButtonPressedAll(const Scene& scene, int button,
                                     int action, int mods) override {
    for (auto& component : behaviours_) {
      component->mouseButtonPressedAll(scene, button, action, mods);
    }
    mouseButtonPressed(scene, button, action, mods);
  }

  virtual void mouseMovedAll(const Scene& scene, double xpos,
                             double ypos) override {
    for (auto& component : behaviours_) {
      component->mouseMovedAll(scene, xpos, ypos);
    }
    mouseMoved(scene, xpos, ypos);
  }
};

}  // namespace engine

#endif
