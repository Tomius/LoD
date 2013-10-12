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
#include "hair.hpp"

extern const float GRAVITY;

class Ayumi {
  oglwrap::AnimatedMesh mesh_;
  oglwrap::Program prog_;
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_, uBones_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  Skybox& skybox_;
public:
  Ayumi(Skybox& skybox, const oglwrap::CharacterMovement& charmove)
    : mesh_("models/ayumi.dae",
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_FlipUVs)
    , fs_("ayumi.frag")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix")
    , uBones_(prog_, "uBones")
    , uSunData_(prog_, "uSunData")
    , skybox_(skybox) {

    oglwrap::ShaderSource vs_source("ayumi.vert");
    vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
    vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());
    vs_.source(vs_source);

    prog_ << vs_ << fs_ << skybox_.sky_fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "vPosition");
    mesh_.setupTexCoords(prog_ | "vTexCoord");
    mesh_.setupNormals(prog_ | "vNormal");
    mesh_.setupBones(prog_ | "vBoneIDs", prog_ | "vWeights");
    oglwrap::UniformSampler(prog_, "uEnvMap").set(0);
    mesh_.setupDiffuseTextures(1);
    mesh_.setupSpecularTextures(2);
    oglwrap::UniformSampler(prog_, "uDiffuseTexture").set(1);
    oglwrap::UniformSampler(prog_, "uSpecularTexture").set(2);

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

  void reshape(glm::mat4 projMat) {
    prog_.use();
    uProjectionMatrix_ = projMat;
  }

  void render(float time,
              const oglwrap::Camera& cam,
              const oglwrap::CharacterMovement& charmove) {
    prog_.use();
    uCameraMatrix_.set(cam.cameraMatrix());
    uModelMatrix_.set(charmove.getModelMatrix() * mesh_.worldTransform());
    uSunData_.set(skybox_.getSunData(time));
    skybox_.env_map.active(0);
    skybox_.env_map.bind();

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
