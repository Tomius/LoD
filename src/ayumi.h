// Copyright (c) 2014, Tamas Csala

#ifndef LOD_INCLUDE_AYUMI_H_
#define LOD_INCLUDE_AYUMI_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"

#include "engine/gameobject.h"
#include "engine/mesh/animated_mesh_renderer.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./shadow.h"

extern const float GRAVITY;
/* 0 -> max quality
   2 -> max performance */
extern const int PERFORMANCE;

class Ayumi : public engine::GameObject {
  engine::AnimatedMeshRenderer mesh_;
  engine::Animation anim_;
  gl::Program prog_, shadow_prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_,
                                  uModelMatrix_, uBones_,
                                  shadow_uMCP_, shadow_uBones_;
  gl::LazyUniform<glm::vec4> uSunData_;

  bool attack2_, attack3_;

  GLFWwindow* window_;

  CharacterMovement *charmove_;
  Skybox* skybox_;
  Shadow* shadow_;

 public:
  Ayumi(GLFWwindow* window, Skybox* skybox, Shadow* shadow);
  virtual ~Ayumi() {}
  engine::AnimatedMeshRenderer& getMesh();
  engine::Animation& getAnimation();
  virtual void update(float time) override;
  virtual void shadowRender(float time, const engine::Camera& cam) override;
  virtual void render(float time, const engine::Camera& cam) override;
  void charmove(CharacterMovement* charmove) {
    charmove_ = (assert(charmove), charmove);
    charmove_->setCanJumpCallback(std::bind(&Ayumi::canJump, this));
    charmove_->setCanFlipCallback(std::bind(&Ayumi::canFlip, this));
  }
  virtual void keyAction(const engine::Timer&, int key, int scancode,
                         int action, int mods) override;

 private:
  CharacterMovement::CanDoCallback canJump;
  CharacterMovement::CanDoCallback canFlip;
  engine::Animation::AnimationEndedCallback animationEndedCallback;
};

#endif  // LOD_INCLUDE_AYUMI_H_
