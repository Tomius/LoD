#include "ayumi.hpp"

using namespace oglwrap;
extern Context gl;
extern bool was_left_click;

Ayumi::Ayumi(Skybox& skybox,
             CharacterMovement& charmove,
             glm::ivec2 shadowAtlasDims)
    : mesh_("models/ayumi/ayumi.dae",
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
    , skybox_(skybox) {

  charmove.setCanJumpCallback(std::bind(&Ayumi::canJump, this));
  charmove.setCanFlipCallback(std::bind(&Ayumi::canFlip, this));

  ShaderSource vs_source("ayumi.vert");
  vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  ShaderSource shadow_vs_source("ayumi_shadow.vert");
  shadow_vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  shadow_vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  VertexShader vs(vs_source), shadow_vs(shadow_vs_source);
  FragmentShader fs("ayumi.frag"), shadow_fs("shadow.frag");

  shadow_prog_ << shadow_vs << shadow_fs;
  shadow_prog_.link();

  shadow_prog_.validate();

  prog_ << vs << fs << skybox_.sky_fs;
  prog_.link().use();

  mesh_.setupPositions(prog_ | "aPosition");
  mesh_.setupTexCoords(prog_ | "aTexCoord");
  mesh_.setupNormals(prog_ | "aNormal");
  LazyVertexAttribArray boneIDs(prog_, "aBoneIDs", false);
  LazyVertexAttribArray weights(prog_, "aWeights", false);
  mesh_.setupBones(boneIDs, weights, false);
  UniformSampler(prog_, "uEnvMap").set(0);

  mesh_.setupDiffuseTextures(1);
  mesh_.setupSpecularTextures(2);
  UniformSampler(prog_, "uDiffuseTexture").set(1);
  UniformSampler(prog_, "uSpecularTexture").set(2);
  UniformSampler(prog_, "uShadowMap").set(3);
  uShadowSoftness_ = 1 << clamp(4 - PERFORMANCE, 0, 4);
  Uniform<glm::ivec2>(prog_, "uShadowAtlasSize").set(shadowAtlasDims);

  prog_.validate();

  using namespace _AnimFlag;

  mesh_.addAnimation(
    "models/ayumi/ayumi_idle.dae", "Stand",
    {Repeat, Interruptable}
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_walk.dae", "Walk",
    {Repeat, Interruptable}
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_walk.dae", "MoonWalk",
    {Repeat, Mirrored, Interruptable}
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_run.dae", "Run",
    {Repeat, Interruptable}
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_jump_rise.dae", "JumpRise",
    {MirroredRepeat, Interruptable}, 0.5f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_jump_fall.dae", "JumpFall",
    {MirroredRepeat, Interruptable}, 0.5f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_flip.dae", "Flip",
    None, 1.5f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_attack.dae", "Attack",
    None, 2.5f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_attack2.dae", "Attack2",
    None, 1.4f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_attack3.dae", "Attack3",
    None, 3.0f
  );

  mesh_.addAnimation(
    "models/ayumi/ayumi_attack_chain0.dae", "Attack_Chain0",
    None, 0.9f
  );

  mesh_.setDefaultAnimation("Stand", 0.3f);
  mesh_.forceAnimToDefault(0);

  mesh_.setAnimationEndedCallback(
    [this](const std::string& str){return animationEndedCallback(str);}
  );
}

AnimatedMesh& Ayumi::getMesh() {
  return mesh_;
}

void Ayumi::resize(glm::mat4 projMat) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Ayumi::updateStatus(float time, const CharacterMovement& charmove) {
  std::string curr_anim = mesh_.getCurrentAnimation();

  if(was_left_click) {
    was_left_click = false;
    if(curr_anim == "Attack") {
      attack2_ = true;
    } else if(curr_anim == "Attack2") {
      attack3_ = true;
    } else if(curr_anim != "Attack3") {
      if(curr_anim == "JumpRise" || curr_anim == "JumpFall") {
        mesh_.forceCurrentAnimation(AnimParams("Attack", 0.2f), time);
      } else {
        mesh_.setCurrentAnimation(AnimParams("Attack", 0.3f), time);
      }
    }
  } else if(charmove.isJumping()) {
    if(charmove.isDoingFlip()) {
      if(curr_anim == "JumpRise") {
        mesh_.setCurrentAnimation(AnimParams("Flip", 0.05f), time);
      } else {
        mesh_.setCurrentAnimation(AnimParams("Flip", 0.2f), time);
      }
    } else if(charmove.isJumpingRise()) {
      mesh_.setCurrentAnimation(AnimParams("JumpRise", 0.3f), time);
    } else {
      mesh_.setCurrentAnimation(AnimParams("JumpFall", 0.3f), time);
    }
  } else {
    if(charmove_.isWalking()) {
      if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        mesh_.setCurrentAnimation(AnimParams("Run", 0.3f), time);
      } else {
        mesh_.setCurrentAnimation(AnimParams("Walk", 0.3f), time);
      }
    } else {
      mesh_.setAnimToDefault(time);
    }
  }

  mesh_.updateBoneInfo(time);
}

void Ayumi::shadowRender(float time, Shadow& shadow,
                         const CharacterMovement& charmove) {
  shadow_prog_.use();
  shadow_uMCP_ = shadow.modelCamProjMat(
    skybox_.getSunPos(time), mesh_.bSphere(),
    charmove.getModelMatrix(), mesh_.worldTransform()
  );
  mesh_.uploadBoneInfo(shadow_uBones_);

  gl.FrontFace(FaceOrientation::CCW);
  auto cullface = Context::TemporaryEnable(Capability::CullFace);
  mesh_.disableTextures();

  mesh_.render();

  mesh_.enableTextures();
  if(shadow.getDepth() < shadow.getMaxDepth()) {
    shadow.push();
  }
}

void Ayumi::render(float time, const Camera& cam,
                   const CharacterMovement& charmove, const Shadow& shadow) {
  prog_.use();
  uCameraMatrix_.set(cam.cameraMatrix());
  uModelMatrix_.set(charmove.getModelMatrix() * mesh_.worldTransform());
  for(size_t i = 0; i < shadow.getDepth(); ++i) {
    uShadowCP_[i] = shadow.shadowCPs()[i];
  }
  uNumUsedShadowMaps_ = shadow.getDepth();
  uSunData_.set(skybox_.getSunData(time));

  skybox_.env_map.active(0);
  skybox_.env_map.bind();
  shadow.shadowTex().active(3);
  shadow.shadowTex().bind();

  mesh_.uploadBoneInfo(uBones_);

  gl.FrontFace(FaceOrientation::CCW);
  auto cullface = Context::TemporaryEnable(Capability::CullFace);

  mesh_.render();

  shadow.shadowTex().active(3);
  shadow.shadowTex().unbind();
  skybox_.env_map.active(0);
  skybox_.env_map.unbind();
}

bool Ayumi::canJump() {
  return mesh_.isInterrupable();
}

bool Ayumi::canFlip() {
  return mesh_.isInterrupable();
}

AnimParams Ayumi::animationEndedCallback(const std::string& current_anim) {

  if(current_anim == "Attack") {
    if(attack2_ || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      return AnimParams("Attack2", 0.1f);
    }
  } else if(current_anim == "Attack2") {
    attack2_ = false;
    if(attack3_ || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      return AnimParams("Attack3", 0.05f);
    }
  } else if(current_anim == "Attack3") {
    attack3_ = false;
  }

  if(current_anim == "Flip") {
    charmove_.setFlip(false);
    return AnimParams("JumpFall", 0.2f);
  }

  if(charmove_.isJumping()) {
    if(charmove_.isJumpingRise()) {
      return AnimParams("JumpRise", 0.3f);
    } else {
      return AnimParams("JumpFall", 0.3f);
    }
  } else if(charmove_.isWalking()) {
    AnimParams params;

    if(current_anim == "Attack2") {
      params.transition_time = 0.4f;
    } else if(current_anim == "Attack_Chain0") {
      params.transition_time = 0.5f;
    } else {
      params.transition_time = 0.3f;
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
      params.name = "Run";
      return params;
    } else {
      params.name = "Walk";
      return params;
    }
  } else {
    AnimParams params;

    if(current_anim == "Attack2") {
      return AnimParams("Stand", 0.4f);
    } else if(current_anim == "Attack_Chain0") {
      return AnimParams("Stand", 0.6f);
    } else {
      return AnimParams("Stand", 0.2f);
    }
  }
}
