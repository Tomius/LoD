// Copyright (c) 2014, Tamas Csala

#include "./behaviour.h"

#define _TRY(YourCode) \
  try { \
    YourCode; \
  } catch (const std::exception& ex) { \
    std::cout << ex.what() << std::endl; \
  }

namespace engine {

void Behaviour::updateAll(const Scene& scene) {
  for (auto& component : behaviours_) {
    component->updateAll(scene);
  }
  _TRY(update(scene));
}

void Behaviour::keyActionAll(const Scene& scene, int key, int scancode,
                          int action, int mods) {
  for (auto& component : behaviours_) {
    component->keyActionAll(scene, key, scancode, action, mods);
  }
  _TRY(keyAction(scene, key, scancode, action, mods));
}

void Behaviour::charTypedAll(const Scene& scene, unsigned codepoint) {
  for (auto& behaviour : behaviours_) {
    behaviour->charTypedAll(scene, codepoint);
  }
  _TRY(charTyped(scene, codepoint));
}

void Behaviour::mouseScrolledAll(const Scene& scene, double xoffset,
                              double yoffset) {
  for (auto& component : behaviours_) {
    component->mouseScrolledAll(scene, xoffset, yoffset);
  }
  _TRY(mouseScrolled(scene, xoffset, yoffset));
}

void Behaviour::mouseButtonPressedAll(const Scene& scene, int button,
                                   int action, int mods) {
  for (auto& component : behaviours_) {
    component->mouseButtonPressedAll(scene, button, action, mods);
  }
  _TRY(mouseButtonPressed(scene, button, action, mods));
}

void Behaviour::mouseMovedAll(const Scene& scene, double xpos,
                           double ypos) {
  for (auto& component : behaviours_) {
    component->mouseMovedAll(scene, xpos, ypos);
  }
  _TRY(mouseMoved(scene, xpos, ypos));
}

}  // namespace engine
