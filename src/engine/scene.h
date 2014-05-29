// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SCENE_H_
#define ENGINE_SCENE_H_

#include <vector>
#include <memory>

#include "../shadow.h"
#include "../skybox.h"

#include "timer.h"
#include "camera.h"

class GameObject;

namespace engine {

class Scene {
  std::vector<std::unique_ptr<GameObject>> gameobjects_, after_effects_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Skybox> skybox_;
  std::unique_ptr<Shadow> shadow_;
  Timer game_time_, environment_time_, camera_time_;

 public:
  virtual ~Scene() {}

  virtual float gravity() { return 9.81f; }

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
  T* addSkybox(Args&&... args) {
    static_assert(std::is_base_of<Skybox, T>::value, "Unknown type");

    auto skybox = new T{std::forward<Args>(args)...};
    skybox_ = std::unique_ptr<Skybox>(skybox);
    return skybox;
  }

  template<typename... Args>
  Skybox* addSkybox(Args&&... args) {
    auto skybox = new Skybox{std::forward<Args>(args)...};
    skybox_ = std::unique_ptr<Skybox>(skybox);
    return skybox;
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

    if (skybox_) {
      skybox_->screenResized(w, h);
    }

    if (camera_) {
      camera_->screenResized(w, h);
    }

    for (auto& i : gameobjects_) {
      i->screenResized(w, h);
    }
  }

private:
  virtual void update() {
    game_time_.tick();
    environment_time_.tick();
    camera_time_.tick();

    if (skybox_) {
      skybox_->update(environment_time_.current);
    }

    for (auto& i : gameobjects_) {
      i->update(game_time_.current);
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
          i->shadowRender(game_time_.current, *camera_);
        }
      } shadow_->end();
    }
  }

  virtual void render() {
    if (!camera_)
      return;

    if (skybox_) {
      skybox_->render(environment_time_.current, *camera_);
    }

    for (auto& i : gameobjects_) {
      i->render(game_time_.current, *camera_);
    }
  }

  virtual void drawGui() {
    for (auto& i : gameobjects_) {
      i->drawGui();
    }
  }

public:
  virtual void turn() {
    update();
    shadowRender();
    render();
    drawGui();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) {
    if (camera_) {
      camera_->keyAction(camera_time_, key, scancode, action, mods);
    }

    for (auto& i : gameobjects_) {
      i->keyAction(game_time_, key, scancode, action, mods);
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
      i->mouseScrolled(game_time_, xoffset, yoffset);
    }
  }

  virtual void mouseButtonPressed(int button, int action, int mods) {
    if (camera_) {
      camera_->mouseButtonPressed(camera_time_, button, action, mods);
    }

    for (auto& i : gameobjects_) {
      i->mouseButtonPressed(game_time_, button, action, mods);
    }
  }

  virtual void mouseMoved(double xpos, double ypos) {
    if (camera_) {
      camera_->mouseMoved(camera_time_, xpos, ypos);
    }

    for (auto& i : gameobjects_) {
      i->mouseMoved(game_time_, xpos, ypos);
    }
  }
};

} // namespace engine


#endif
