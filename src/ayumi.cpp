#include "ayumi.hpp"

using namespace oglwrap;
extern Context gl;
extern bool was_left_click;

Ayumi::Ayumi(Skybox& skybox, CharacterMovement& charmove, Shadow& shadow)
    : mesh_("models/ayumi/ayumi.dae",
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_FlipUVs)
    , anim_(mesh_.getAnimData())
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
    , skybox_(skybox)
    , shadow_(shadow) {

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
  Uniform<glm::ivec2>(prog_, "uShadowAtlasSize").set(shadow.getAtlasDimensions());

  prog_.validate();

  using namespace engine::_AnimFlag;

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

  anim_.setDefaultAnimation("Stand", 0.3f);
  anim_.forceAnimToDefault(0);

  anim_.setAnimationEndedCallback(
    [this](const std::string& str){return animationEndedCallback(str);}
  );
}

engine::AnimatedMeshRenderer& Ayumi::getMesh() {
  return mesh_;
}

engine::Animation& Ayumi::getAnimation() {
  return anim_;
}

void Ayumi::screenResized(const glm::mat4& projMat, GLuint, GLuint) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Ayumi::update(float time) {
  charmove_.update(time);

  std::string curr_anim = anim_.getCurrentAnimation();

  using engine::AnimParams;

  if(was_left_click) {
    was_left_click = false;
    if(curr_anim == "Attack") {
      attack2_ = true;
    } else if(curr_anim == "Attack2") {
      attack3_ = true;
    } else if(curr_anim != "Attack3") {
      if(curr_anim == "JumpRise" || curr_anim == "JumpFall") {
        anim_.forceCurrentAnimation(AnimParams("Attack", 0.2f), time);
      } else {
        anim_.setCurrentAnimation(AnimParams("Attack", 0.3f), time);
      }
    }
  } else if(charmove_.isJumping()) {
    if(charmove_.isDoingFlip()) {
      if(curr_anim == "JumpRise") {
        anim_.setCurrentAnimation(AnimParams("Flip", 0.05f), time);
      } else {
        anim_.setCurrentAnimation(AnimParams("Flip", 0.2f), time);
      }
    } else if(charmove_.isJumpingRise()) {
      anim_.setCurrentAnimation(AnimParams("JumpRise", 0.3f), time);
    } else {
      anim_.setCurrentAnimation(AnimParams("JumpFall", 0.3f), time);
    }
  } else {
    if(charmove_.isWalking()) {
      if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
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

void Ayumi::shadowRender(float time, const engine::Camera& cam) {
  shadow_prog_.use();
  shadow_uMCP_ = shadow_.modelCamProjMat(
    skybox_.getSunPos(), mesh_.bSphere(),
    charmove_.getModelMatrix(), mesh_.worldTransform()
  );
  mesh_.uploadBoneInfo(shadow_uBones_);

  gl.FrontFace(FaceOrientation::CCW);
  auto cullface = Context::TemporaryEnable(Capability::CullFace);
  mesh_.disableTextures();

  mesh_.render();

  mesh_.enableTextures();
  shadow_.push();
}

void Ayumi::render(float time, const engine::Camera& cam) {
  prog_.use();
  uCameraMatrix_.set(cam.cameraMatrix());
  uModelMatrix_.set(charmove_.getModelMatrix() * mesh_.worldTransform());
  for(size_t i = 0; i < shadow_.getDepth(); ++i) {
    uShadowCP_[i] = shadow_.shadowCPs()[i];
  }
  uNumUsedShadowMaps_ = shadow_.getDepth();
  uSunData_.set(skybox_.getSunData());

  skybox_.env_map.active(0);
  skybox_.env_map.bind();
  shadow_.shadowTex().active(3);
  shadow_.shadowTex().bind();

  mesh_.uploadBoneInfo(uBones_);

  gl.FrontFace(FaceOrientation::CCW);
  auto cullface = Context::TemporaryEnable(Capability::CullFace);

  mesh_.render();

  shadow_.shadowTex().active(3);
  shadow_.shadowTex().unbind();
  skybox_.env_map.active(0);
  skybox_.env_map.unbind();
}

bool Ayumi::canJump() {
  return anim_.isInterrupable();
}

bool Ayumi::canFlip() {
  return anim_.isInterrupable();
}

engine::AnimParams Ayumi::animationEndedCallback(const std::string& current_anim) {

  using engine::AnimParams;

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
