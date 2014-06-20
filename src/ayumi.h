// Copyright (c) 2014, Tamas Csala

#ifndef LOD_INCLUDE_AYUMI_H_
#define LOD_INCLUDE_AYUMI_H_

#include "engine/oglwrap_config.h"
#include "engine/behaviour.h"
#include "engine/mesh/animated_mesh_renderer.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./shadow.h"

class Ayumi : public engine::Behaviour {
  engine::AnimatedMeshRenderer mesh_;
  engine::Animation anim_;
  gl::Program prog_, shadow_prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_,
                                  uModelMatrix_, uBones_,
                                  shadow_uMCP_, shadow_uBones_;
  gl::LazyUniform<glm::vec3> uSunPos_;

  bool attack2_, attack3_;

  GLFWwindow* window_;

  CharacterMovement *charmove_;
  Skybox* skybox_;
  Shadow* shadow_;
  bool was_left_click_;

  glm::vec4 bsphere_;

  CharacterMovement::CanDoCallback canJump;
  CharacterMovement::CanDoCallback canFlip;
  engine::Animation::AnimationEndedCallback animationEndedCallback;

 public:
  Ayumi(GLFWwindow* window, Skybox* skybox, Shadow* shadow);
  virtual ~Ayumi() {}
  engine::AnimatedMeshRenderer& getMesh();
  engine::Animation& getAnimation();
  virtual void update(const engine::Scene& scene) override;
  virtual void shadowRender(const engine::Scene& scene) override;
  virtual void render(const engine::Scene& scene) override;
  void charmove(CharacterMovement* charmove) {
    charmove_ = (assert(charmove), charmove);
    charmove_->setCanJumpCallback(std::bind(&Ayumi::canJump, this));
    charmove_->setCanFlipCallback(std::bind(&Ayumi::canFlip, this));
  }
  virtual void mouseButtonPressed(const engine::Scene& scene, int button,
                                  int action, int mods) override;
};

#endif  // LOD_INCLUDE_AYUMI_H_
