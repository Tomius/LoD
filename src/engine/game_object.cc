// Copyright (c) 2014, Tamas Csala

#include "./game_object.h"
#include "./behaviour.h"

namespace engine {

void GameObject::shadowRenderAll(const Scene& scene) {
  shadowRender(scene);
  for (auto& component : components_) {
    component->shadowRenderAll(scene);
  }
}

void GameObject::renderAll(const Scene& scene) {
  render(scene);
  for (auto& component : components_) {
    component->renderAll(scene);
  }
}

void GameObject::render2DAll(const Scene& scene) {
  render2D(scene);
  for (auto& component : components_) {
    component->render2DAll(scene);
  }
}

void GameObject::screenResizedAll(const Scene& scene, size_t width,
                                  size_t height) {
  for (auto& behaviour : components_) {
    behaviour->screenResizedAll(scene, width, height);
  }
  screenResized(scene, width, height);
}

void GameObject::updateAll(const Scene& scene) {
  for (auto& behaviour : behaviours_) {
    behaviour->updateAll(scene);
  }
}

void GameObject::keyActionAll(const Scene& scene, int key, int scancode,
                              int action, int mods) {
  for (auto& behaviour : behaviours_) {
    behaviour->keyActionAll(scene, key, scancode, action, mods);
  }
}

void GameObject::mouseScrolledAll(const Scene& scene, double xoffset,
                                  double yoffset) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseScrolledAll(scene, xoffset, yoffset);
  }
}

void GameObject::mouseButtonPressedAll(const Scene& scene, int button,
                                       int action, int mods) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseButtonPressedAll(scene, button, action, mods);
  }
}

void GameObject::mouseMovedAll(const Scene& scene, double xpos, double ypos) {
  for (auto& behaviour : behaviours_) {
    behaviour->mouseMovedAll(scene, xpos, ypos);
  }
}

}  // namespace engine
