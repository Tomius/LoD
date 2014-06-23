// Copyright (c) 2014, Tamas Csala

#ifndef LOD_INCLUDE_AYUMI_H_
#define LOD_INCLUDE_AYUMI_H_

#include "engine/behaviour.h"
#include "engine/shader_manager.h"
#include "engine/mesh/animated_mesh_renderer.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./shadow.h"

class Ayumi : public engine::Behaviour {
 public:
  explicit Ayumi(engine::Scene* scene);
  virtual ~Ayumi() {}

  engine::AnimatedMeshRenderer& getMesh();
  engine::Animation& getAnimation();

  void charmove(CharacterMovement* charmove) {
    if (!charmove) return;
    charmove_ = charmove;
    charmove_->setCanJumpCallback(std::bind(&Ayumi::canJump, this));
    charmove_->setCanFlipCallback(std::bind(&Ayumi::canFlip, this));
  }

 private:
  engine::AnimatedMeshRenderer mesh_;
  engine::Animation anim_;
  engine::ShaderProgram prog_, shadow_prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_,
                             uModelMatrix_, uBones_,
                             shadow_uMCP_, shadow_uBones_;

  bool attack2_, attack3_, was_left_click_;
  CharacterMovement *charmove_;

  glm::vec4 bsphere_;

  CharacterMovement::CanDoCallback canJump;
  CharacterMovement::CanDoCallback canFlip;
  engine::Animation::AnimationEndedCallback animationEndedCallback;

  engine::ShaderFile* loadVertexShader(engine::ShaderManager* manager);
  engine::ShaderFile* loadShadowVertexShader(engine::ShaderManager* manager);

  virtual void update() override;
  virtual void shadowRender() override;
  virtual void render() override;
  virtual void mouseButtonPressed(int button, int action, int mods) override;
};

#endif  // LOD_INCLUDE_AYUMI_H_
