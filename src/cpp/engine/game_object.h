// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <set>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

#include "./transform.h"

namespace engine {

class Scene;

class GameObject {
 public:
  template<typename Transform_t = Transform>
  explicit GameObject(GameObject* parent,
                      const Transform_t& initial_transform = Transform_t{});
  virtual ~GameObject() {}

  template<typename T, typename... Args>
  T* addComponent(Args&&... contructor_args);
  GameObject* addComponent(std::unique_ptr<GameObject>&& component);

  // Returns the first component found by depth first search in the
  // GameObject hierarchy whose type is T
  template<typename T>
  T* findComponent() const { return FindComponent<T>(this); }

  // Returns all the components in the GameObject heirarchy whose type is T
  template<typename T>
  std::vector<T*> findComponents() const;

  // Detaches a componenent from its parent, and adopts it.
  // Returns true on success.
  bool stealComponent(GameObject* component_to_steal);

  void removeComponent(GameObject* component_to_remove);

  template <typename T>
  void removeComponents(T begin, T end);

  Transform* transform() { return transform_.get(); }
  const Transform* transform() const { return transform_.get(); }

  GameObject* parent() { return parent_; }
  const GameObject* parent() const { return parent_; }
  void set_parent(GameObject* parent);

  Scene* scene() { return scene_; }
  const Scene* scene() const { return scene_; }
  void set_scene(Scene* scene) { scene_ = scene; }

  bool enabled() const { return enabled_; }
  void set_enabled(bool value);

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
  virtual void collisionAll(const GameObject* other);

 protected:
  Scene* scene_;
  GameObject* parent_;
  std::unique_ptr<Transform> transform_;
  std::vector<std::unique_ptr<GameObject>> components_;
  std::vector<GameObject*> components_just_enabled_, components_just_disabled_;

  struct CompareGameObjects {
    bool operator() (GameObject* x, GameObject* y) const;
  };

  std::set<GameObject*, CompareGameObjects> sorted_components_;
  int uid_, group_;
  bool enabled_;

  void internalUpdate();

 private:
  template<typename T>
  static T* FindComponent(const GameObject* obj);

  template<typename T>
  static void FindComponents(const GameObject* obj, std::vector<T*> *found);

  static int NextUid();

  struct ComponentRemoveHelper {
    std::set<GameObject*> components_;
    bool operator()(const std::unique_ptr<GameObject>& go_ptr) {
      return components_.find(go_ptr.get()) != components_.end();
    }
  } remove_predicate_;

  void updateSortedComponents();
  void removeComponents();
};

}  // namespace engine

#include "./game_object-inl.h"

#endif
