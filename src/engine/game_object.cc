// Copyright (c) 2014, Tamas Csala

#include "./behaviour.h"

#define _TRY(YourCode) \
  try { \
    YourCode; \
  } catch (const std::exception& ex) { \
    std::cout << ex.what() << std::endl; \
  }

namespace engine {

void GameObject::shadowRenderAll() {
  _TRY(shadowRender());
  for (auto& component : components_) {
    component->shadowRenderAll();
  }
}

void GameObject::renderAll() {
  _TRY(render());
  for (auto& component : components_) {
    component->renderAll();
  }
}

void GameObject::render2DAll() {
  _TRY(render2D());
  for (auto& component : components_) {
    component->render2DAll();
  }
}

void GameObject::screenResizedAll(size_t width, size_t height) {
  for (auto& behaviour : components_) {
    behaviour->screenResizedAll(width, height);
  }
  _TRY(screenResized(width, height));
}

void GameObject::updateAll() {
  for (auto& behaviour : behaviours_) {
    behaviour->updateAll();
  }
}

void GameObject::keyActionAll(int key, int scancode, int action, int mods) {
  for (auto& behaviour : behaviours_) {
    behaviour->keyActionAll(key, scancode, action, mods);
  }
}

void GameObject::charTypedAll(unsigned codepoint) {
  for (auto& behaviour : behaviours_) {
    behaviour->charTypedAll(codepoint);
  }
}

void GameObject::mouseScrolledAll(double xoffset, double yoffset) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseScrolledAll(xoffset, yoffset);
  }
}

void GameObject::mouseButtonPressedAll(int button, int action, int mods) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseButtonPressedAll(button, action, mods);
  }
}

void GameObject::mouseMovedAll(double xpos, double ypos) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseMovedAll(xpos, ypos);
  }
}

}  // namespace engine
