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
  GameObject* parent_;
  std::vector<std::unique_ptr<GameObject>> components_;
  std::vector<Behaviour*> behaviours_;

 public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  virtual ~GameObject() {}

  void addRigidBody(const HeightMapInterface& height_map,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
        new RigidBody {transform, height_map, starting_height});
  }

  template<typename T, typename... Args>
  T* addComponent(Args&&... args) {
    static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

    T *obj = new T {std::forward<Args>(args)...};
    transform.addChild(obj->transform);
    obj->parent_ = this;
    components_.push_back(std::unique_ptr<GameObject>(obj));

    // is_base_of wonldn't work as Behaviour is incomplete here
    Behaviour* behaviour = dynamic_cast<Behaviour*>(obj);
    if (behaviour) {
      behaviours_.push_back(behaviour);
    }

    return obj;
  }

  GameObject* parent() {return parent_;}
  const GameObject* parent() const {return parent_;}

  virtual void shadowRender(const Scene& scene) {}
  virtual void render(const Scene& scene) {}
  virtual void render2D(const Scene& scene) {}
  virtual void screenResized(const Scene& scene, size_t width, size_t height) {}

  virtual void shadowRenderAll(const Scene& scene);
  virtual void renderAll(const Scene& scene);
  virtual void render2DAll(const Scene& scene);
  virtual void screenResizedAll(const Scene& scene, size_t width, size_t height);

  virtual void updateAll(const Scene& scene);
  virtual void keyActionAll(const Scene& scene, int key, int scancode,
                            int action, int mods);
  virtual void charTypedAll(const Scene& scene, unsigned codepoint);
  virtual void mouseScrolledAll(const Scene& scene, double xoffset,
                                double yoffset);
  virtual void mouseButtonPressedAll(const Scene& scene, int button,
                                     int action, int mods);
  virtual void mouseMovedAll(const Scene& scene, double xpos, double ypos);
};
}  // namespace engine

#endif
