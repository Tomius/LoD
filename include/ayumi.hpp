#ifndef LOD_AYUMI_HPP_
#define LOD_AYUMI_HPP_

#if defined(__APPLE__)
#include <OpenGL/glew.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>
#endif

#include "oglwrap/oglwrap.hpp"
#include "oglwrap/mesh/animatedMesh.hpp"
#include "oglwrap/utils/camera.hpp"

#include "charmove.hpp"
#include "skybox.hpp"
#include "shadow.hpp"

extern const float GRAVITY;

class Ayumi {
  oglwrap::AnimatedMesh mesh_;
  oglwrap::Program prog_, shadow_prog_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_, uBones_, uShadowCP_;
  oglwrap::LazyUniform<glm::mat4> shadow_uMCP_, shadow_uBones_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<int> uNumUsedShadowMaps_;

  Skybox& skybox_;
public:
  Ayumi(Skybox& skybox, const oglwrap::CharacterMovement& charmove)
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

    mesh_.setupPositions(prog_ | "vPosition");
    mesh_.setupTexCoords(prog_ | "vTexCoord");
    mesh_.setupNormals(prog_ | "vNormal");
    mesh_.setupBones(prog_ | "vBoneIDs", prog_ | "vWeights");
    oglwrap::UniformSampler(prog_, "uEnvMap").set(0);
    mesh_.setupDiffuseTextures(1);
    mesh_.setupSpecularTextures(2);
    oglwrap::UniformSampler(prog_, "uDiffuseTexture").set(1);
    oglwrap::UniformSampler(prog_, "uSpecularTexture").set(2);
    oglwrap::UniformSampler(prog_, "uShadowMap").set(3);

    mesh_.addAnimation("models/ayumi_idle.dae", "Stand",
                       oglwrap::AnimFlag::Repeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh_.addAnimation("models/ayumi_walk.dae", "Walk",
                       oglwrap::AnimFlag::Repeat |
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

  oglwrap::AnimatedMesh& getMesh() {
    return mesh_;
  }

  void resize(glm::mat4 projMat) {
    prog_.use();
    uProjectionMatrix_ = projMat;
  }

  void updateStatus(float time, const oglwrap::CharacterMovement& charmove) {
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

  void shadowRender(float time,
                    Shadow& shadow,
                    const oglwrap::CharacterMovement& charmove) {
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

  void render(float time,
              const oglwrap::Camera& cam,
              const oglwrap::CharacterMovement& charmove,
              const Shadow& shadow) {
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
};

#endif // LOD_AYUMI_HPP_
