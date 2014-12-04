// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SCENE_H_
#define ENGINE_SCENE_H_

#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "./oglwrap_config.h"
#include "../oglwrap/oglwrap.h"

#include "./timer.h"
#include "./camera.h"
#include "./game_object.h"
#include "./behaviour.h"
#include "./shader_manager.h"
#include "./auto_reset_event.h"

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

    // close the physics thread
    physics_thread_should_quit_ = true;
    physics_can_run_.set();
    physics_thread_.join();
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

  const Camera* camera() const { return camera_; }
  Camera* camera() { return camera_; }
  void set_camera(Camera* camera) { camera_ = camera; }

  const Shadow* shadow() const { return shadow_; }
  Shadow* shadow() { return shadow_; }
  void set_shadow(Shadow* shadow) { shadow_ = shadow; }

  ShaderManager* shader_manager();

  GLFWwindow* window() const { return window_; }
  void set_window(GLFWwindow* window) { window_ = window; }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
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

  virtual void turn() {
    physics_finished_.waitOne();
    updateAll();
    physics_can_run_.set();
    shadowRenderAll();
    renderAll();
    render2DAll();
  }

 protected:
  // Bullet classes
  std::unique_ptr<btCollisionConfiguration> collision_config_;
  std::unique_ptr<btDispatcher> dispatcher_;
  std::unique_ptr<btBroadphaseInterface> broadphase_;
  std::unique_ptr<btConstraintSolver> solver_;
  std::unique_ptr<btDynamicsWorld> world_;

  // physics thread data
  AutoResetEvent physics_can_run_{false}, physics_finished_{true};
  bool physics_thread_should_quit_;
  std::thread physics_thread_;

  // Own data
  Camera* camera_;
  Shadow* shadow_;
  Timer game_time_, environment_time_, camera_time_;
  GLFWwindow* window_;

  virtual void updateAll() override {
    game_time_.tick();
    environment_time_.tick();
    camera_time_.tick();

    Behaviour::updateAll();
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
    gl::TemporarySet capabilities{{{gl::kBlend, true},
                                   {gl::kCullFace, false},
                                   {gl::kDepthTest, false}}};
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    Behaviour::render2DAll();
  }

  virtual void updatePhysics() {
    if (world_) {
      world_->stepSimulation(game_time().dt, 0);
    }
  }
};

}  // namespace engine


#endif
