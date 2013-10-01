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

class Ayumi {
  oglwrap::AnimatedMesh mesh_;
  oglwrap::Program prog_;
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;

  oglwrap::LazyUniform<glm::mat4> projectionMatrix_, cameraMatrix_, modelMatrix_, bones_;
  oglwrap::LazyUniform<glm::vec4> sunData_;

  Skybox& skybox_;
public:
  Ayumi(Skybox& skybox)
    : mesh_("models/ayumi.dae",
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_FlipUVs)
    , fs_("ayumi.frag")
    , projectionMatrix_(prog_, "ProjectionMatrix")
    , cameraMatrix_(prog_, "CameraMatrix")
    , modelMatrix_(prog_, "ModelMatrix")
    , bones_(prog_, "Bones")
    , sunData_(prog_, "SunData")
    , skybox_(skybox) {

    oglwrap::ShaderSource vs_source("ayumi.vert");
    vs_source.insertMacroValue("BONE_ATTRIB_NUM", mesh_.getBoneAttribNum());
    vs_source.insertMacroValue("BONE_NUM", mesh_.getNumBones());
    vs_.source(vs_source);

    prog_ << vs_ << fs_ << skybox_.sky_fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "Position");
    mesh_.setupTexCoords(prog_ | "TexCoord");
    mesh_.setupNormals(prog_ | "Normal");
    mesh_.setupBones(prog_ | "BoneIDs", prog_ | "Weights");
    oglwrap::UniformSampler(prog_, "EnvMap").set(0);
    mesh_.setupDiffuseTextures(1);
    mesh_.setupSpecularTextures(2);
    oglwrap::UniformSampler(prog_, "DiffuseTexture").set(1);
    oglwrap::UniformSampler(prog_, "SpecularTexture").set(2);

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
                       oglwrap::AnimFlag::None, 2.0f);

    mesh_.setDefaultAnimation("Stand", 0.3f);
    mesh_.forceAnimToDefault(0);
  }

  oglwrap::AnimatedMesh& getMesh() {
    return mesh_;
  }

  void reshape(glm::mat4 projMat) {
    prog_.use();
    projectionMatrix_ = projMat;
  }

  void render(float time,
              const oglwrap::Camera& cam,
              const oglwrap::CharacterMovement& charmove) {
    prog_.use();
    cameraMatrix_.set(cam.cameraMatrix());
    modelMatrix_.set(charmove.getModelMatrix());
    sunData_.set(skybox_.getSunData(time));
    skybox_.envMap.active(0);
    skybox_.envMap.bind();

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

    mesh_.boneTransform(time, bones_);
    mesh_.render();
    skybox_.envMap.active(0);
    skybox_.envMap.unbind();
  }
};

#endif // LOD_AYUMI_HPP_
