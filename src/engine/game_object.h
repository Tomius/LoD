// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <set>
#include <memory>
#include <vector>
#include <algorithm>

#include "./camera.h"

namespace engine {

class Scene;
class Behaviour;

class GameObject {
 public:
  Transform transform;

  explicit GameObject(GameObject* parent);
  virtual ~GameObject() {}

  template<typename T, typename... Args>
  T* addComponent(Args&&... args);

  GameObject* parent() { return parent_; }
  const GameObject* parent() const { return parent_; }
  void set_parent(GameObject* parent);

  Scene* scene() { return scene_; }
  const Scene* scene() const { return scene_; }
  void set_scene(Scene* scene) { scene_ = scene; }

  bool enabled() const { return enabled_; }
  void set_enabled(bool value);

  int layer() const { return layer_; }
  void set_layer(int value) { layer_ = value; }

  int group() const { return group_; }
  void set_group(int value);

  virtual void shadowRender() {}
  virtual void render() {}
  virtual void render2D() {}
  virtual void screenResized(size_t width, size_t height) {}

  virtual void shadowRenderAll();
  virtual void renderAll();
  virtual void render2DAll();
  virtual void screenResizedAll(size_t width, size_t height);

  virtual void updateAll();
  virtual void keyActionAll(int key, int scancode, int action, int mods);
  virtual void charTypedAll(unsigned codepoint);
  virtual void mouseScrolledAll(double xoffset, double yoffset);
  virtual void mouseButtonPressedAll(int button, int action, int mods);
  virtual void mouseMovedAll(double xpos, double ypos);

 protected:
  Scene* scene_;
  GameObject* parent_;
  std::vector<std::unique_ptr<GameObject>> components_;
  std::vector<GameObject*> components_just_enabled_, components_just_disabled_;

  struct CompareGameObjects {
    bool operator() (GameObject* x, GameObject* y) const;
  };

  std::multiset<GameObject*, CompareGameObjects> sorted_components_;
  int layer_, group_;
  bool enabled_;

  void updateSortedComponents();
};

}  // namespace engine

#include "./game_object-inl.h"

#endif
