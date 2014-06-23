// Copyright (c) 2014, Tamas Csala

#include "./behaviour.h"

#define _TRY(YourCode) \
  try { \
    YourCode; \
  } catch (const std::exception& ex) { \
    std::cout << ex.what() << std::endl; \
  }

namespace engine {

void Behaviour::updateAll() {
  for (auto& component : behaviours_) {
    component->updateAll();
  }
  _TRY(update());
}

void Behaviour::keyActionAll(int key, int scancode, int action, int mods) {
  for (auto& component : behaviours_) {
    component->keyActionAll(key, scancode, action, mods);
  }
  _TRY(keyAction(key, scancode, action, mods));
}

void Behaviour::charTypedAll(unsigned codepoint) {
  for (auto& behaviour : behaviours_) {
    behaviour->charTypedAll(codepoint);
  }
  _TRY(charTyped(codepoint));
}

void Behaviour::mouseScrolledAll(double xoffset, double yoffset) {
  for (auto& component : behaviours_) {
    component->mouseScrolledAll(xoffset, yoffset);
  }
  _TRY(mouseScrolled(xoffset, yoffset));
}

void Behaviour::mouseButtonPressedAll(int button, int action, int mods) {
  for (auto& component : behaviours_) {
    component->mouseButtonPressedAll(button, action, mods);
  }
  _TRY(mouseButtonPressed(button, action, mods));
}

void Behaviour::mouseMovedAll(double xpos, double ypos) {
  for (auto& component : behaviours_) {
    component->mouseMovedAll(xpos, ypos);
  }
  _TRY(mouseMoved(xpos, ypos));
}

}  // namespace engine
