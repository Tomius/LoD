// Copyright (c) 2014, Tamas Csala

#include "./ayumi.h"

#include <string>
#include "engine/oglwrap_config.h"
#include <GLFW/glfw3.h>

#include "engine/scene.h"

using engine::AnimParams;

engine::ShaderFile* Ayumi::loadVertexShader(engine::ShaderManager* manager) {
  gl::ShaderSource vs_src("ayumi.vert");
  vs_src.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  vs_src.insertMacroValue("BONE_NUM", mesh_.getNumBones());
  return manager->publish("ayumi.vert", vs_src);
}

engine::ShaderFile* Ayumi::loadShadowVertexShader(
    engine::ShaderManager* manager) {
  gl::ShaderSource shadow_vs_src("ayumi_shadow.vert");
  shadow_vs_src.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  shadow_vs_src.insertMacroValue("BONE_NUM", mesh_.getNumBones());
  return manager->publish("ayumi_shadow.vert", shadow_vs_src);
}


Ayumi::Ayumi(engine::GameObject* parent)
    : engine::Behaviour(parent)
    , mesh_("models/ayumi/ayumi.dae",
            aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs)
    , anim_(mesh_.getAnimData())
    , prog_(loadVertexShader(scene_->shader_manager()),
            scene_->shader_manager()->get("ayumi.frag"))
    , shadow_prog_(loadShadowVertexShader(scene_->shader_manager()),
                   scene_->shader_manager()->get("shadow.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix")
    , uBones_(prog_, "uBones")
    , shadow_uMCP_(shadow_prog_, "uMCP")
    , shadow_uBones_(shadow_prog_, "uBones")
    , attack2_(false)
    , attack3_(false)
    , was_left_click_(false)
    , charmove_(nullptr)
    , bsphere_(mesh_.bSphere()) {
  gl::Use(prog_);

  mesh_.setupPositions(prog_ | "aPosition");
  mesh_.setupTexCoords(prog_ | "aTexCoord");
  mesh_.setupNormals(prog_ | "aNormal");
  gl::LazyVertexAttrib boneIDs(prog_, "aBoneIDs", false);
  gl::LazyVertexAttrib weights(prog_, "aWeights", false);
  mesh_.setupBones(boneIDs, weights, false);

  mesh_.setupDiffuseTextures();
  gl::UniformSampler(prog_, "uDiffuseTexture").set(0);
  mesh_.setupSpecularTextures();
  gl::UniformSampler(prog_, "uSpecularTexture").set(1);

  prog_.validate();

  using engine::AnimFlag;

  mesh_.addAnimation("models/ayumi/ayumi_idle.dae", "Stand",
                     {AnimFlag::Repeat, AnimFlag::Interruptable});

  mesh_.addAnimation("models/ayumi/ayumi_walk.dae", "Walk",
                     {AnimFlag::Repeat, AnimFlag::Interruptable});

  mesh_.addAnimation("models/ayumi/ayumi_walk.dae", "MoonWalk",
                     {AnimFlag::Repeat, AnimFlag::Mirrored,
                     AnimFlag::Interruptable});

  mesh_.addAnimation("models/ayumi/ayumi_run.dae", "Run",
                     {AnimFlag::Repeat, AnimFlag::Interruptable});

  mesh_.addAnimation("models/ayumi/ayumi_jump_rise.dae", "JumpRise",
                     {AnimFlag::MirroredRepeat, AnimFlag::Interruptable}, 0.5f);

  mesh_.addAnimation("models/ayumi/ayumi_jump_fall.dae", "JumpFall",
                     {AnimFlag::MirroredRepeat, AnimFlag::Interruptable}, 0.5f);

  mesh_.addAnimation("models/ayumi/ayumi_flip.dae", "Flip",
                     AnimFlag::None, 1.5f);

  mesh_.addAnimation("models/ayumi/ayumi_attack.dae", "Attack",
                     AnimFlag::None, 2.5f);

  mesh_.addAnimation("models/ayumi/ayumi_attack2.dae", "Attack2",
                     AnimFlag::None, 1.4f);

  mesh_.addAnimation("models/ayumi/ayumi_attack3.dae", "Attack3",
                     AnimFlag::None, 3.0f);

  mesh_.addAnimation("models/ayumi/ayumi_attack_chain0.dae", "Attack_Chain0",
                     AnimFlag::None, 0.9f);

  anim_.setDefaultAnimation("Stand", 0.3f);
  anim_.forceAnimToDefault(0);

  anim_.setAnimationEndedCallback(
    [this](const std::string& str){return animationEndedCallback(str);});
}

engine::AnimatedMeshRenderer& Ayumi::getMesh() {
  return mesh_;
}

engine::Animation& Ayumi::getAnimation() {
  return anim_;
}

void Ayumi::update() {
  float time = scene_->game_time().current;

  std::string curr_anim = anim_.getCurrentAnimation();

  using engine::AnimParams;

  if (was_left_click_) {
    was_left_click_ = false;
    if (curr_anim == "Attack") {
      attack2_ = true;
    } else if (curr_anim == "Attack2") {
      attack3_ = true;
    } else if (curr_anim != "Attack3") {
      if (curr_anim == "JumpRise" || curr_anim == "JumpFall") {
        anim_.forceCurrentAnimation(AnimParams("Attack", 0.2f), time);
      } else {
        anim_.setCurrentAnimation(AnimParams("Attack", 0.3f), time);
      }
    }
  } else if (charmove_->isJumping()) {
    if (charmove_->isDoingFlip()) {
      if (curr_anim == "JumpRise") {
        anim_.setCurrentAnimation(AnimParams("Flip", 0.05f), time);
      } else {
        anim_.setCurrentAnimation(AnimParams("Flip", 0.2f), time);
      }
    } else if (charmove_->isJumpingRise()) {
      anim_.setCurrentAnimation(AnimParams("JumpRise", 0.3f), time);
    } else {
      anim_.setCurrentAnimation(AnimParams("JumpFall", 0.3f), time);
    }
  } else {
    if (charmove_->isWalking()) {
      if (glfwGetKey(scene_->window(), GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        anim_.setCurrentAnimation(AnimParams("Run", 0.3f), time);
      } else {
        anim_.setCurrentAnimation(AnimParams("Walk", 0.3f), time);
      }
    } else {
      anim_.setAnimToDefault(time);
    }
  }

  mesh_.updateBoneInfo(anim_, time);
}

void Ayumi::shadowRender() {
  gl::Use(shadow_prog_);
  shadow_uMCP_ =
    scene_->shadow()->modelCamProjMat(bsphere_, transform()->matrix(),
                                     mesh_.worldTransform());
  mesh_.uploadBoneInfo(shadow_uBones_);

  gl::CullFace(gl::kFront);
  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};
  mesh_.disableTextures();

  mesh_.render();

  mesh_.enableTextures();
  gl::CullFace(gl::kBack);

  scene_->shadow()->push();
}

void Ayumi::render() {
  gl::Use(prog_);
  prog_.update();
  const auto& cam = *scene_->camera();
  uCameraMatrix_ = cam.cameraMatrix();
  uProjectionMatrix_ = cam.projectionMatrix();
  uModelMatrix_ = transform()->matrix() * mesh_.worldTransform();

  mesh_.uploadBoneInfo(uBones_);

  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};

  mesh_.render();
}

bool Ayumi::canJump() {
  return anim_.isInterrupable();
}

bool Ayumi::canFlip() {
  return anim_.isInterrupable();
}

AnimParams Ayumi::animationEndedCallback(const std::string& current_anim) {
  if (current_anim == "Attack") {
    if (attack2_ || glfwGetMouseButton(scene_->window(), GLFW_MOUSE_BUTTON_LEFT)
          == GLFW_PRESS) {
      return AnimParams("Attack2", 0.1f);
    }
  } else if (current_anim == "Attack2") {
    attack2_ = false;
    if (attack3_ || glfwGetMouseButton(scene_->window(), GLFW_MOUSE_BUTTON_LEFT)
          == GLFW_PRESS) {
      return AnimParams("Attack3", 0.05f);
    }
  } else if (current_anim == "Attack3") {
    attack3_ = false;
  }

  if (current_anim == "Flip") {
    charmove_->setFlip(false);
    return AnimParams("JumpFall", 0.2f);
  }

  if (charmove_->isJumping()) {
    if (charmove_->isJumpingRise()) {
      return AnimParams("JumpRise", 0.3f);
    } else {
      return AnimParams("JumpFall", 0.3f);
    }
  } else if (charmove_->isWalking()) {
    AnimParams params;

    if (current_anim == "Attack2") {
      params.transition_time = 0.4f;
    } else if (current_anim == "Attack_Chain0") {
      params.transition_time = 0.5f;
    } else {
      params.transition_time = 0.3f;
    }
    if (glfwGetKey(scene_->window(), GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
      params.name = "Run";
      return params;
    } else {
      params.name = "Walk";
      return params;
    }
  } else {
    AnimParams params;

    if (current_anim == "Attack2") {
      return AnimParams("Stand", 0.4f);
    } else if (current_anim == "Attack_Chain0") {
      return AnimParams("Stand", 0.6f);
    } else {
      return AnimParams("Stand", 0.2f);
    }
  }
}


void Ayumi::mouseButtonPressed(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    was_left_click_ = true;
  }
}
