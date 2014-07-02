// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SCENE_H_
#define ENGINE_SCENE_H_

#include <vector>
#include <memory>
#include <bullet/btBulletDynamicsCommon.h>

#include "./oglwrap_config.h"
#include "../oglwrap/oglwrap.h"

#include "./timer.h"
#include "./camera.h"
#include "./game_object.h"
#include "./behaviour.h"
#include "./shader_manager.h"

#include "../shadow.h"

namespace engine {

class GameObject;

class Scene : public Behaviour {
 public:
  Scene();
  virtual ~Scene() {
    // The GameObject's destructor have to run here
    // as they might use the scene ptr in their destructor
    for (auto& comp_ptr : components_) {
      comp_ptr.reset();
    }
  }

  virtual float gravity() const { return 9.81f; }

  const btDynamicsWorld* world() const { return world_.get(); }
  btDynamicsWorld* world() { return world_.get(); }

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

  ShaderManager* shader_manager() { return &shader_manager_; }

  GLFWwindow* window() const { return window_; }
  void set_window(GLFWwindow* window) { window_ = window; }

  GameObject* addGameObject() { return addComponent<GameObject>(); }

  template<typename T, typename... Args>
  T* addShadow(Args&&... args) {
    static_assert(std::is_base_of<Shadow, T>::value, "Unknown type");

    auto shadow = new T(std::forward<Args>(args)...);
    shadow_ = std::unique_ptr<Shadow>(shadow);
    return shadow;
  }

  template<typename... Args>
  Shadow* addShadow(Args&&... args) {
    auto shadow = new Shadow(std::forward<Args>(args)...);
    shadow_ = std::unique_ptr<Shadow>(shadow);
    return shadow;
  }

  template<typename T, typename... Args>
  T* addCamera(Args&&... args) {
    static_assert(std::is_base_of<Camera, T>::value, "Unknown type");

    auto camera = new T(std::forward<Args>(args)...);
    camera_ = std::unique_ptr<Camera>(camera);
    return camera;
  }

  virtual void screenResized(size_t w, size_t h) override {
    if (shadow_) {
      shadow_->screenResized(w, h);
    }

    if (camera_) {
      camera_->screenResized(w, h);
    }
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (camera_) {
      camera_->keyAction(camera_time_, key, scancode, action, mods);
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

  virtual void mouseScrolled(double xoffset, double yoffset) override {
    if (camera_) {
      camera_->mouseScrolled(camera_time_, xoffset, yoffset);
    }
  }

  virtual void mouseButtonPressed(int button, int action, int mods) override {
    if (camera_) {
      camera_->mouseButtonPressed(camera_time_, button, action, mods);
    }
  }

  virtual void mouseMoved(double xpos, double ypos) override {
    if (camera_) {
      camera_->mouseMoved(camera_time_, xpos, ypos);
    }
  }

  virtual void turn() {
    updateAll();
    shadowRenderAll();
    renderAll();
    render2DAll();
  }

  int target_layer() const { return target_layer_; }
  void set_target_layer(int value) { target_layer_ = value; }

 protected:
  // Bullet classes
  std::unique_ptr<btCollisionConfiguration> collision_config_;
  std::unique_ptr<btDispatcher> dispatcher_;
  std::unique_ptr<btBroadphaseInterface> broadphase_;
  std::unique_ptr<btConstraintSolver> solver_;
  std::unique_ptr<btDynamicsWorld> world_;

  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Shadow> shadow_;
  ShaderManager shader_manager_;
  Timer game_time_, environment_time_, camera_time_;
  GLFWwindow* window_;
  int target_layer_;

  virtual void updateAll() override {
    game_time_.tick();
    environment_time_.tick();
    camera_time_.tick();

    Behaviour::updateAll();

    if (camera_) {
      camera_->update(camera_time_);
    }
  }

  virtual void shadowRenderAll() override {
    if (camera_ && shadow_) {
      shadow_->begin(); {
        Behaviour::shadowRenderAll();
      } shadow_->end();
    }
  }

  virtual void renderAll() override {
    if (camera_) { Behaviour::renderAll(); }
  }

  virtual void render2DAll() override {
    auto capabilities = gl::TemporarySet({{gl::kBlend, true},
                                          {gl::kCullFace, false},
                                          {gl::kDepthTest, false}});
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    Behaviour::render2DAll();
  }
};

}  // namespace engine


#endif
