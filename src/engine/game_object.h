// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <memory>
#include "./camera.h"
#include "./rigid_body.h"

namespace engine {

class Scene;

class GameObject {
  std::vector<std::unique_ptr<GameObject>> components_;
public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  virtual ~GameObject() {}

  void addRigidBody(const HeightMapInterface& height_map,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
      new RigidBody{transform, height_map, starting_height}
    );
  }

  template<typename T, typename... Args>
  T* addComponent(Args&&... args) {
    static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

    T *go = new T{std::forward<Args>(args)...};
    components_.push_back(std::unique_ptr<GameObject>(go));
    return go;
  }

  virtual void update(const Scene& scene) {}
  virtual void shadowRender(const Scene& scene) {}
  virtual void render(const Scene& scene) {}
  virtual void render2D(const Scene& scene) {}

  virtual void screenResized(const Scene& scene, size_t width, size_t height) {}
  virtual void keyAction(const Scene& scene, int key, int scancode,
                                             int action, int mods) {}
  virtual void mouseScrolled(const Scene& scene, double xoffset,
                                                 double yoffset) {}
  virtual void mouseButtonPressed(const Scene& scene, int button,
                                  int action, int mods) {}
  virtual void mouseMoved(const Scene& scene, double xpos, double ypos) {}

  void updateAll(const Scene& scene) {
    for (auto& component : components_) {
      component->updateAll(scene);
    }
    update(scene);
  }

  void shadowRenderAll(const Scene& scene) {
    shadowRender(scene);
    for (auto& component : components_) {
      component->shadowRenderAll(scene);
    }
  }

  void renderAll(const Scene& scene) {
    render(scene);
    for (auto& component : components_) {
      component->renderAll(scene);
    }
  }

  void render2DAll(const Scene& scene) {
    render2D(scene);
    for (auto& component : components_) {
      component->render2DAll(scene);
    }
  }

  void screenResizedAll(const Scene& scene, size_t width, size_t height) {
    for (auto& component : components_) {
      component->screenResizedAll(scene, width, height);
    }
    screenResized(scene, width, height);
  }

  void keyActionAll(const Scene& scene, int key, int scancode,
                                  int action, int mods) {
    for (auto& component : components_) {
      component->keyActionAll(scene, key, scancode, action, mods);
    }
    keyAction(scene, key, scancode, action, mods);
  }

  void mouseScrolledAll(const Scene& scene, double xoffset,
                                      double yoffset) {
    for (auto& component : components_) {
      component->mouseScrolledAll(scene, xoffset, yoffset);
    }
    mouseScrolled(scene, xoffset, yoffset);
  }

  void mouseButtonPressedAll(const Scene& scene, int button,
                             int action, int mods) {
    for (auto& component : components_) {
      component->mouseButtonPressedAll(scene, button, action, mods);
    }
    mouseButtonPressed(scene, button, action, mods);
  }

  void mouseMovedAll(const Scene& scene, double xpos, double ypos) {
    for (auto& component : components_) {
      component->mouseMovedAll(scene, xpos, ypos);
    }
    mouseMoved(scene, xpos, ypos);
  }
};

}


#endif
