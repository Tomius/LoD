// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <memory>
#include <vector>
#include <algorithm>

#include "./camera.h"
#include "./rigid_body.h"

namespace engine {

class Scene;
class Behaviour;

class GameObject {
 public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  explicit GameObject(Scene* scene)
      : scene_(scene), enabled_(true), layer_(0), group_(0) {
    sorted_components_.push_back(this);
  }
  virtual ~GameObject() {}

  void addRigidBody(const HeightMapInterface& height_map,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
        new RigidBody(transform, height_map, starting_height));
  }

  template<typename T, typename... Args>
  T* addComponent(Args&&... args) {
    static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

    try {
      T *obj = new T(scene_, std::forward<Args>(args)...);
      transform.addChild(obj->transform);
      obj->parent_ = this;
      components_.push_back(std::unique_ptr<GameObject>(obj));
      sorted_components_.push_back(obj);
      sort_components();

      return obj;
    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return nullptr;
    }
  }

  GameObject* parent() { return parent_; }
  const GameObject* parent() const { return parent_; }

  Scene* scene() { return scene_; }
  const Scene* scene() const { return scene_; }
  void set_scene(Scene* scene) { scene_ = scene; }

  bool enabled() const { return enabled_; }
  void set_enabled(bool value) {
    enabled_ = value;
    sort_components();
    if (parent_) {
      parent_->sort_components();
    }
  }

  int layer() const { return layer_; }
  void set_layer(int value) { layer_ = value; }

  int group() const { return group_; }
  void set_group(int value) {
    group_ = value;
    sort_components();
    if (parent_) {
      parent_->sort_components();
    }
  }

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
  std::vector<GameObject*> sorted_components_;
  bool enabled_;
  int layer_, group_;

  struct CompareGameObjects {
    bool operator() (GameObject* x, GameObject* y) const {
      if (x->enabled() == true) {
        if (y->enabled() == true) {
          if (x->group() == y->group()) {
            if (y->parent() == x) {
              return true;  // we should render the parent before the children
            } else {
              return false;
            }
          } else {
            return x->group() < y->group();
          }
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  };

  void sort_components() {
    std::sort(sorted_components_.begin(),
              sorted_components_.end(),
              CompareGameObjects{});
  }
};

}  // namespace engine

#endif
