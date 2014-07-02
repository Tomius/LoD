// Copyright (c) 2014, Tamas Csala

#include "./game_object.h"

#define _TRY(YourCode) \
  try { \
    YourCode; \
  } catch (const std::exception& ex) { \
    std::cout << ex.what() << std::endl; \
  }

namespace engine {

void GameObject::shadowRenderAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(shadowRender());
    } else {
      component->shadowRenderAll();
    }
  }
}

void GameObject::renderAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(render());
    } else {
      component->renderAll();
    }
  }
}

void GameObject::render2DAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(render2D());
    } else {
      component->render2DAll();
    }
  }
}

void GameObject::screenResizedAll(size_t width, size_t height) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY(screenResized(width, height));
    } else {
      component->screenResizedAll(width, height);
    }
  }
}

void GameObject::updateAll() {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->updateAll();
    }
  }
}

void GameObject::keyActionAll(int key, int scancode, int action, int mods) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->keyActionAll(key, scancode, action, mods);
    }
  }
}

void GameObject::charTypedAll(unsigned codepoint) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->charTypedAll(codepoint);
    }
  }
}

void GameObject::mouseScrolledAll(double xoffset, double yoffset) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->mouseScrolledAll(xoffset, yoffset);
    }
  }
}

void GameObject::mouseButtonPressedAll(int button, int action, int mods) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->mouseButtonPressedAll(button, action, mods);
    }
  }
}

void GameObject::mouseMovedAll(double xpos, double ypos) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->mouseMovedAll(xpos, ypos);
    }
  }
}

}  // namespace engine
