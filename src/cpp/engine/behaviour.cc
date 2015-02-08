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
  internalUpdate();
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(update());
    } else {
      component->updateAll();
    }
  }
}

void Behaviour::keyActionAll(int key, int scancode, int action, int mods) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(keyAction(key, scancode, action, mods));
    } else {
      component->keyActionAll(key, scancode, action, mods);
    }
  }
}

void Behaviour::charTypedAll(unsigned codepoint) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(charTyped(codepoint));
    } else {
      component->charTypedAll(codepoint);
    }
  }
}

void Behaviour::mouseScrolledAll(double xoffset, double yoffset) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(mouseScrolled(xoffset, yoffset));
    } else {
      component->mouseScrolledAll(xoffset, yoffset);
    }
  }
}

void Behaviour::mouseButtonPressedAll(int button, int action, int mods) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(mouseButtonPressed(button, action, mods));
    } else {
      component->mouseButtonPressedAll(button, action, mods);
    }
  }
}

void Behaviour::mouseMovedAll(double xpos, double ypos) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(mouseMoved(xpos, ypos));
    } else {
      component->mouseMovedAll(xpos, ypos);
    }
  }
}

void Behaviour::collisionAll(const GameObject* other) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(collision(other));
    } else {
      component->collisionAll(other);
    }
  }
}

}  // namespace engine
