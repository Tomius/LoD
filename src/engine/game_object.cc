// Copyright (c) 2014, Tamas Csala

#include "./scene.h"
#include "./game_object.h"
#include "./game_engine.h"

#define _TRY_(YourCode) \
  try { \
    YourCode; \
  } catch (const std::exception& ex) { \
    std::cout << ex.what() << std::endl; \
  } catch (...) {}

namespace engine {

GameObject* GameObject::addComponent(std::unique_ptr<GameObject>&& component) {
  try {
    GameObject *obj = component.get();
    components_.push_back(std::move(component));
    components_just_enabled_.push_back(obj);

    return obj;
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return nullptr;
  }
}

void GameObject::set_parent(GameObject* parent) {
  parent_ = parent;
  if (parent) { transform_->set_parent(parent_->transform()); }
}

void GameObject::set_enabled(bool value) {
  enabled_ = value;
  if (value) {
    components_just_enabled_.push_back(this);
    if (parent_) {
      parent_->components_just_enabled_.push_back(this);
    }
  } else {
    components_just_disabled_.push_back(this);
    if (parent_) {
      parent_->components_just_disabled_.push_back(this);
    }
  }
}

void GameObject::set_group(int value) {
  group_ = value;
  components_just_enabled_.push_back(this);
  components_just_disabled_.push_back(this);
  if (parent_) {
    parent_->components_just_enabled_.push_back(this);
    parent_->components_just_disabled_.push_back(this);
  }
}

void GameObject::shadowRenderAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY_(shadowRender());
    } else {
      component->shadowRenderAll();
    }
  }
}

void GameObject::renderAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY_(render());
    } else {
      component->renderAll();
    }
  }
}

void GameObject::render2DAll() {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY_(render2D());
    } else {
      component->render2DAll();
    }
  }
}

void GameObject::screenResizedAll(size_t width, size_t height) {
  for (auto& component : sorted_components_) {
    if (component == this) {
      _TRY_(screenResized(width, height));
    } else {
      component->screenResizedAll(width, height);
    }
  }
}

void GameObject::updateAll() {
  updateSortedComponents();
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

void GameObject::collisionAll(const GameObject* other) {
  for (auto& component : sorted_components_) {
    if (component != this) {
      component->collisionAll(other);
    }
  }
}


void GameObject::updateSortedComponents() {
  for (const auto& element : components_just_disabled_) {
    sorted_components_.erase(element);
  }
  components_just_disabled_.clear();
  sorted_components_.insert(components_just_enabled_.begin(),
                            components_just_enabled_.end());
  // make sure all the componenets just enabled are aware of the screen's size
  glm::vec2 window_size = GameEngine::window_size();
  for (const auto& component : components_just_enabled_) {
    component->screenResizedAll(window_size.x, window_size.y);
  }
  components_just_enabled_.clear();
}

bool GameObject::CompareGameObjects::operator()(GameObject* x,
                                                GameObject* y) const {
  if (x->group() == y->group()) {
    if (y->parent() == x) {
      return true;  // we should render the parent before the children
    } else {
      return false;
    }
  } else {
    return x->group() < y->group();
  }
}

}  // namespace engine
