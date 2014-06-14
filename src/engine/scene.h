// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SCENE_H_
#define ENGINE_SCENE_H_

#include <vector>
#include <memory>

#include "../shadow.h"
#include "../oglwrap/oglwrap.h"

#include "timer.h"
#include "camera.h"

namespace engine {

class GameObject;

class Scene {
  std::vector<std::unique_ptr<GameObject>> gameobjects_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Shadow> shadow_;
  Timer game_time_, environment_time_, camera_time_;

 public:
  virtual ~Scene() {}

  // setters and getters
  virtual float gravity() const { return 9.81f; }
  const Timer& game_time() const { return game_time_; }
  Timer& game_time() { return game_time_; }
  const Timer& environment_time() const { return environment_time_; }
  Timer& environment_time() { return environment_time_; }
  const Timer& camera_time() const { return camera_time_; }
  Timer& camera_time() { return camera_time_; }
  const Camera* camera() const { return camera_.get(); }
  Camera* camera() { return camera_.get(); }
  const Shadow* shadow() const { return shadow_.get(); }
  Shadow* shadow() { return shadow_.get(); }

  template<typename T, typename... Args>
  T* addGameObject(Args&&... args) {
    static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

    T *go = new T{std::forward<Args>(args)...};
    gameobjects_.push_back(std::unique_ptr<GameObject>(go));
    return go;
  }

  template<typename... Args>
  GameObject* addGameObject(Args&&... args) {
    GameObject *go = new GameObject{std::forward<Args>(args)...};
    gameobjects_.push_back(std::unique_ptr<GameObject>(go));
    return go;
  }

  template<typename T, typename... Args>
  T* addShadow(Args&&... args) {
    static_assert(std::is_base_of<Shadow, T>::value, "Unknown type");

    auto shadow = new T{std::forward<Args>(args)...};
    shadow_ = std::unique_ptr<Shadow>(shadow);
    return shadow;
  }

  template<typename... Args>
  Shadow* addShadow(Args&&... args) {
    auto shadow = new Shadow{std::forward<Args>(args)...};
    shadow_ = std::unique_ptr<Shadow>(shadow);
    return shadow;
  }

  template<typename T, typename... Args>
  T* addCamera(Args&&... args) {
    static_assert(std::is_base_of<Camera, T>::value, "Unknown type");

    auto camera = new T{std::forward<Args>(args)...};
    camera_ = std::unique_ptr<Camera>(camera);
    return camera;
  }

  virtual void screenResized(size_t w, size_t h) {
    if (shadow_) {
      shadow_->screenResized(w, h);
    }

    if (camera_) {
      camera_->screenResized(w, h);
    }

    for (auto& i : gameobjects_) {
      i->screenResizedAll(*this, w, h);
    }
  }

private:
  virtual void update() {
    game_time_.tick();
    environment_time_.tick();
    camera_time_.tick();

    for (auto& i : gameobjects_) {
      i->updateAll(*this);
    }

    if (camera_) {
      camera_->update(camera_time_);
    }
  }

  virtual void shadowRender() {
    if (!camera_)
      return;

    if (shadow_) {
      shadow_->begin(); {
        for (auto& i : gameobjects_) {
          i->shadowRenderAll(*this);
        }
      } shadow_->end();
    }
  }

  virtual void render() {
    if (!camera_)
      return;

    for (auto& i : gameobjects_) {
      i->renderAll(*this);
    }
  }

  virtual void render2D() {
    auto capabilities = gl::TemporarySet({{gl::kBlend, true},
                                          {gl::kCullFace, false},
                                          {gl::kDepthTest, false}});
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    for (auto& i : gameobjects_) {
      i->render2DAll(*this);
    }
  }

 public:
  virtual void turn() {
    update();
    shadowRender();
    render();
    render2D();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) {
    if (camera_) {
      camera_->keyAction(camera_time_, key, scancode, action, mods);
    }

    for (auto& i : gameobjects_) {
      i->keyActionAll(*this, key, scancode, action, mods);
    }

    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_F1:
          game_time_.toggle();
          break;
        case GLFW_KEY_F2:
          environment_time_.toggle();
          break;
        default:
          break;
      }
    }
  }

  virtual void mouseScrolled(double xoffset, double yoffset) {
    if (camera_) {
      camera_->mouseScrolled(camera_time_, xoffset, yoffset);
    }

    for (auto& i : gameobjects_) {
      i->mouseScrolledAll(*this, xoffset, yoffset);
    }
  }

  virtual void mouseButtonPressed(int button, int action, int mods) {
    if (camera_) {
      camera_->mouseButtonPressed(camera_time_, button, action, mods);
    }

    for (auto& i : gameobjects_) {
      i->mouseButtonPressedAll(*this, button, action, mods);
    }
  }

  virtual void mouseMoved(double xpos, double ypos) {
    if (camera_) {
      camera_->mouseMoved(camera_time_, xpos, ypos);
    }

    for (auto& i : gameobjects_) {
      i->mouseMovedAll(*this, xpos, ypos);
    }
  }
};

} // namespace engine


#endif
