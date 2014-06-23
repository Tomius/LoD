// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <memory>
#include <vector>

#include "./camera.h"
#include "./rigid_body.h"

namespace engine {

class Scene;
class Behaviour;

class GameObject {
 protected:
  Scene* scene_;
  GameObject* parent_;
  std::vector<std::unique_ptr<GameObject>> components_;
  std::vector<Behaviour*> behaviours_;

 public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  explicit GameObject(Scene* scene) : scene_(scene) {}
  virtual ~GameObject() {}

  void addRigidBody(const HeightMapInterface& height_map,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
        new RigidBody {transform, height_map, starting_height});
  }

  template<typename T, typename... Args>
  T* addComponent(Args&&... args) {
    static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

    try {
      T *obj = new T {scene_, std::forward<Args>(args)...};
      transform.addChild(obj->transform);
      obj->parent_ = this;
      components_.push_back(std::unique_ptr<GameObject>(obj));

      // is_base_of wouldn't work as Behaviour is incomplete here
      Behaviour* behaviour = dynamic_cast<Behaviour*>(obj);
      if (behaviour) {
        behaviours_.push_back(behaviour);
      }

      return obj;
    } catch (const std::exception& ex) {
      std::cout << ex.what() << std::endl;
      return nullptr;
    }
  }

  GameObject* parent() { return parent_; }
  const GameObject* parent() const { return parent_; }

  Scene* scene() { return scene_; }
  const Scene* scene() const { return scene_; }
  void set_scene(Scene* scene) { scene_ = scene; }

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
};
}  // namespace engine

#endif
