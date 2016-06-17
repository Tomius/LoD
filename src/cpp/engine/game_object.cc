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
  if (component == nullptr) {
    return nullptr;
  }

  try {
    GameObject *obj = component.get();
    components_.push_back(std::move(component));
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
  if (parent) {
    transform_->set_parent(parent_->transform());
  } else {
    transform_->set_parent(nullptr);
  }
}

void GameObject::set_enabled(bool value) {
  enabled_ = value;
}

void GameObject::shadowRenderAll() {
  shadowRender();
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->shadowRenderAll();
  }
}

void GameObject::renderAll() {
  render();
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->renderAll();
  }
}

void GameObject::render2DAll() {
  render2D();
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->render2DAll();
  }
}

void GameObject::screenResizedAll(size_t width, size_t height) {
  screenResized(width, height);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->screenResizedAll(width, height);
  }
}

void GameObject::updateAll() {
  update();
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->updateAll();
  }
}

void GameObject::keyActionAll(int key, int scancode, int action, int mods) {
  keyAction(key, scancode, action, mods);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->keyActionAll(key, scancode, action, mods);
  }
}

void GameObject::charTypedAll(unsigned codepoint) {
  charTyped(codepoint);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->charTypedAll(codepoint);
  }
}

void GameObject::mouseScrolledAll(double xoffset, double yoffset) {
  mouseScrolled(xoffset, yoffset);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->mouseScrolledAll(xoffset, yoffset);
  }
}

void GameObject::mouseButtonPressedAll(int button, int action, int mods) {
  mouseButtonPressed(button, action, mods);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->mouseButtonPressedAll(button, action, mods);
  }
}

void GameObject::mouseMovedAll(double xpos, double ypos) {
  mouseMoved(xpos, ypos);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->mouseMovedAll(xpos, ypos);
  }
}

void GameObject::collisionAll(const GameObject* other) {
  collision(other);
  for (size_t i = 0; i < components_.size(); ++i) {
    components_[i]->collisionAll(other);
  }
}

void GameObject::InitScreenSize() {
  glm::vec2 window_size = GameEngine::window_size();
  screenResized(window_size.x, window_size.y);
}

}  // namespace engine
