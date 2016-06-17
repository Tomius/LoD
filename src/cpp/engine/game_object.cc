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

GameObject* GameObject::addComponent(const std::shared_ptr<GameObject>& component) {
  try {
    GameObject *obj = component.get();
    components_.push_back(component);
    obj->parent_ = this;
    obj->transform_->set_parent(transform_.get());
    obj->scene_ = scene_;
    // make sure that the object is aware of the screen's size
    obj->InitScreenSize();


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
}

void GameObject::shadowRenderAll() {
  shadowRender();
  for (auto& component : components_copy_) {
    component->shadowRenderAll();
  }
}

void GameObject::renderAll() {
  render();
  for (auto& component : components_copy_) {
    component->renderAll();
  }
}

void GameObject::render2DAll() {
  render2D();
  for (auto& component : components_copy_) {
    component->render2DAll();
  }
}

void GameObject::screenResizedAll(size_t width, size_t height) {
  screenResized(width, height);
  for (auto& component : components_copy_) {
    component->screenResizedAll(width, height);
  }
}

void GameObject::updateAll() {
  components_copy_.clear();
  components_copy_.reserve(components_.size());
  for (auto& component : components_) {
    if (component->enabled_) {
      components_copy_.push_back(component);
    }
  }

  for (auto& component : components_copy_) {
    component->updateAll();
  }
}

void GameObject::keyActionAll(int key, int scancode, int action, int mods) {
  for (auto& component : components_copy_) {
    component->keyActionAll(key, scancode, action, mods);
  }
}

void GameObject::charTypedAll(unsigned codepoint) {
  for (auto& component : components_copy_) {
    component->charTypedAll(codepoint);
  }
}

void GameObject::mouseScrolledAll(double xoffset, double yoffset) {
  for (auto& component : components_copy_) {
    component->mouseScrolledAll(xoffset, yoffset);
  }
}

void GameObject::mouseButtonPressedAll(int button, int action, int mods) {
  for (auto& component : components_copy_) {
    component->mouseButtonPressedAll(button, action, mods);
  }
}

void GameObject::mouseMovedAll(double xpos, double ypos) {
  for (auto& component : components_copy_) {
    component->mouseMovedAll(xpos, ypos);
  }
}

void GameObject::collisionAll(const GameObject* other) {
  for (auto& component : components_copy_) {
    component->collisionAll(other);
  }
}

void GameObject::InitScreenSize() {
  glm::vec2 window_size = GameEngine::window_size();
  screenResized(window_size.x, window_size.y);
}

}  // namespace engine
