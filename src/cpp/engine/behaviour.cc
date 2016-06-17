// Copyright (c) 2014, Tamas Csala

#include "./behaviour.h"

namespace engine {

void Behaviour::updateAll() {
  components_copy_.clear();
  components_copy_.reserve(components_.size());
  for (auto& component : components_) {
    if (component->enabled()) {
      components_copy_.push_back(component);
    }
  }

  update ();
  for (auto& component : components_copy_) {
    component->updateAll();
  }
}

void Behaviour::keyActionAll(int key, int scancode, int action, int mods) {
  keyAction(key, scancode, action, mods);
  for (auto& component : components_copy_) {
    component->keyActionAll(key, scancode, action, mods);
  }
}

void Behaviour::charTypedAll(unsigned codepoint) {
  charTyped(codepoint);
  for (auto& component : components_copy_) {
    component->charTypedAll(codepoint);
  }
}

void Behaviour::mouseScrolledAll(double xoffset, double yoffset) {
  mouseScrolled(xoffset, yoffset);
  for (auto& component : components_copy_) {
    component->mouseScrolledAll(xoffset, yoffset);
  }
}

void Behaviour::mouseButtonPressedAll(int button, int action, int mods) {
  mouseButtonPressed(button, action, mods);
  for (auto& component : components_copy_) {
    component->mouseButtonPressedAll(button, action, mods);
  }
}

void Behaviour::mouseMovedAll(double xpos, double ypos) {
  mouseMoved(xpos, ypos);
  for (auto& component : components_copy_) {
    component->mouseMovedAll(xpos, ypos);
  }
}

void Behaviour::collisionAll(const GameObject* other) {
  collision(other);
  for (auto& component : components_copy_) {
    component->collisionAll(other);
  }
}

}  // namespace engine
