#include "ayumi.hpp"

extern bool was_left_click;

Ayumi::Ayumi(Skybox& skybox, CharacterMovement& charmove)
    : mesh_("models/ayumi.dae",
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_FlipUVs)
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix")
    , uBones_(prog_, "uBones")
    , uShadowCP_(prog_, "uShadowCP")
    , shadow_uMCP_(shadow_prog_, "uMCP")
    , shadow_uBones_(shadow_prog_, "uBones")
    , uSunData_(prog_, "uSunData")
    , uNumUsedShadowMaps_(prog_, "uNumUsedShadowMaps")
    , uShadowSoftness_(prog_, "uShadowSoftness")
    , attack2_(false)
    , charmove_(charmove)
    , anim_end_listener_(*this)
    , skybox_(skybox) {

  oglwrap::ShaderSource vs_source("ayumi.vert");
  vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  oglwrap::ShaderSource shadow_vs_source("ayumi_shadow.vert");
  shadow_vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  shadow_vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  oglwrap::VertexShader vs(vs_source), shadow_vs(shadow_vs_source);
  oglwrap::FragmentShader fs("ayumi.frag"), shadow_fs("shadow.frag");

  prog_ << vs << fs << skybox_.sky_fs;
  prog_.link().use();

  shadow_prog_ << shadow_vs << shadow_fs;
  shadow_prog_.link();

  mesh_.setupPositions(prog_ | "aPosition");
  mesh_.setupTexCoords(prog_ | "aTexCoord");
  mesh_.setupNormals(prog_ | "aNormal");
  mesh_.setupBones(prog_ | "aBoneIDs", prog_ | "aWeights");
  oglwrap::UniformSampler(prog_, "uEnvMap").set(0);

  mesh_.setupDiffuseTextures(1);
  mesh_.setupSpecularTextures(2);
  oglwrap::UniformSampler(prog_, "uDiffuseTexture").set(1);
  oglwrap::UniformSampler(prog_, "uSpecularTexture").set(2);
  oglwrap::UniformSampler(prog_, "uShadowMap").set(3);
  uShadowSoftness_ = 1 << clamp(4 - PERFORMANCE, 0, 4);

  mesh_.addAnimation("models/ayumi_idle.dae", "Stand",
                     oglwrap::AnimFlag::Repeat |
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_walk.dae", "Walk",
                     oglwrap::AnimFlag::Repeat |
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_walk.dae", "MoonWalk",
                     oglwrap::AnimFlag::Repeat |
                     oglwrap::AnimFlag::Mirrored |
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_run.dae", "Run",
                     oglwrap::AnimFlag::Repeat |
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_jump_rise.dae", "JumpRise",
                     oglwrap::AnimFlag::MirroredRepeat | 
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_jump_fall.dae", "JumpFall",
                     oglwrap::AnimFlag::MirroredRepeat | 
                     oglwrap::AnimFlag::Interruptable);

  mesh_.addAnimation("models/ayumi_flip.dae", "Flip",
                     oglwrap::AnimFlag::None, 1.2);

  mesh_.addAnimation("models/ayumi_attack.dae", "Attack",
                     oglwrap::AnimFlag::None, 2.5f);

  mesh_.addAnimation("models/ayumi_attack2.dae", "Attack2",
                     oglwrap::AnimFlag::None, 1.4f);

  mesh_.addAnimation("models/ayumi_attack3.dae", "Attack3",
                     oglwrap::AnimFlag::None, 3.0f);

  mesh_.setDefaultAnimation("Stand", 0.3f);
  mesh_.forceAnimToDefault(0);

  mesh_.setAnimationEndedCallback(&anim_end_listener_);
}

oglwrap::AnimatedMesh& Ayumi::getMesh() {
  return mesh_;
}

void Ayumi::resize(glm::mat4 projMat) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Ayumi::updateStatus(float time, const CharacterMovement& charmove) {
  if(was_left_click) {
    was_left_click = false;
    if(mesh_.getCurrentAnimation() == "Attack") {
      attack2_ = true;
    } else if(mesh_.getCurrentAnimation() == "Attack2") {
      attack3_ = true;
    } else if(mesh_.getCurrentAnimation() != "Attack3") {
      mesh_.setCurrentAnimation("Attack", time, 0.3f);
    }
  } else if(charmove.isJumping()) {
    if(charmove.isDoingFlip()) {
      mesh_.setCurrentAnimation("Flip", time, 0.1f);
    } else if(charmove.isJumpingRise()) {
      mesh_.setCurrentAnimation("JumpRise", time, 0.3f);
    } else {
      mesh_.setCurrentAnimation("JumpFall", time, 0.3f);
    }
  } else {
    if(charmove_.isWalking()) {
      if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        if(mesh_.getCurrentAnimation() != "Run") {
          mesh_.setCurrentAnimation("Run", time, 0.3f);
        }
      } else {
        if(mesh_.getCurrentAnimation() != "Walk") {
          mesh_.setCurrentAnimation("Walk", time, 0.3f);
        }
      }
    } else {
      if(mesh_.getCurrentAnimation() != "Stand") {
        mesh_.setAnimToDefault(time);
      }
    }
  }

  mesh_.updateBoneInfo(time);
}

void Ayumi::shadowRender(float time, Shadow& shadow, const CharacterMovement& charmove) {
  shadow_prog_.use();
  shadow_uMCP_ = shadow.modelCamProjMat(
                   skybox_.getSunPos(time), mesh_.bSphere(), charmove.getModelMatrix()
                 );
  mesh_.uploadBoneInfo(shadow_uBones_);

  gl(FrontFace(GL_CCW));
  gl(Enable(GL_CULL_FACE));
  mesh_.render();
  gl(Disable(GL_CULL_FACE));

  if(shadow.getDepth() < shadow.getMaxDepth()) {
    shadow.push();
  }
}

void Ayumi::render(float time, const oglwrap::Camera& cam,
                   const CharacterMovement& charmove, const Shadow& shadow) {
  prog_.use();
  uCameraMatrix_.set(cam.cameraMatrix());
  uModelMatrix_.set(charmove.getModelMatrix() * mesh_.worldTransform());
  for(int i = 0; i < shadow.getDepth(); ++i) {
    uShadowCP_[i] = shadow.shadowCPs()[i];
  }
  uNumUsedShadowMaps_ = shadow.getDepth();
  uSunData_.set(skybox_.getSunData(time));
  skybox_.env_map.active(0);
  skybox_.env_map.bind();
  shadow.shadowTex().active(3);
  shadow.shadowTex().bind();

  mesh_.uploadBoneInfo(uBones_);

  gl(FrontFace(GL_CCW));
  gl(Enable(GL_CULL_FACE));
  mesh_.render();
  gl(Disable(GL_CULL_FACE));
  skybox_.env_map.active(0);
  skybox_.env_map.unbind();
}
