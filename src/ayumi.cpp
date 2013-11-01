#include "ayumi.hpp"

Ayumi::Ayumi(Skybox& skybox, const CharacterMovement& charmove)
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
  , skybox_(skybox) {

  oglwrap::ShaderSource vs_source("ayumi.vert");
  vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  oglwrap::ShaderSource shadow_vs_source("ayumi_shadow.vert");
  shadow_vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
  shadow_vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());

  prog_.attachShader(oglwrap::VertexShader(vs_source));
  prog_.attachShader(oglwrap::FragmentShader("ayumi.frag"));
  prog_.attachShader(skybox_.sky_fs);
  prog_.link().use();

  shadow_prog_.attachShader(oglwrap::VertexShader(shadow_vs_source));
  shadow_prog_.attachShader(oglwrap::FragmentShader("shadow.frag"));
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

  mesh_.addAnimation("models/ayumi_attack.dae", "Attack",
                     oglwrap::AnimFlag::None, 2.5f);

  mesh_.setDefaultAnimation("Stand", 0.3f);
  mesh_.forceAnimToDefault(0);
}

oglwrap::AnimatedMesh& Ayumi::getMesh() {
  return mesh_;
}

void Ayumi::resize(glm::mat4 projMat) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Ayumi::updateStatus(float time, const CharacterMovement& charmove) {
  if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    mesh_.setCurrentAnimation("Attack", time, 0.3f);
  } else if(charmove.is_jumping()) {
    if(charmove.is_jumping_rise()) {
      mesh_.setCurrentAnimation("JumpRise", time, 0.3f);
    } else {
      mesh_.setCurrentAnimation("JumpFall", time, 0.3f);
    }
  } else if(charmove.isWalking()) {
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
      mesh_.setCurrentAnimation("Run", time, 0.2f);
    } else {
      mesh_.setCurrentAnimation("Walk", time, 0.3f);
    }
  } else {
    mesh_.setAnimToDefault(time);
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
