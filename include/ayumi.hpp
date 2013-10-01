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
public:
  oglwrap::AnimatedMesh mesh;
private:
  oglwrap::Program prog;
  oglwrap::VertexShader vs;
  oglwrap::FragmentShader fs;

  oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix, modelMatrix, bones;
  oglwrap::LazyUniform<glm::vec4> sunData;

  Skybox& skybox;
public:
  Ayumi(Skybox& skybox)
    : mesh("models/ayumi.dae",
           aiProcessPreset_TargetRealtime_MaxQuality |
           aiProcess_FlipUVs)
    , fs("ayumi.frag")
    , projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , modelMatrix(prog, "ModelMatrix")
    , bones(prog, "Bones")
    , sunData(prog, "SunData")
    , skybox(skybox) {

    oglwrap::ShaderSource vs_source("ayumi.vert");
    vs_source.insert_macro_value("BONE_ATTRIB_NUM", mesh.getBoneAttribNum());
    vs_source.insert_macro_value("BONE_NUM", mesh.getNumBones());
    vs.source(vs_source);

    prog << vs << fs << skybox.sky_fs;
    prog.link().use();

    mesh.setupPositions(prog | "Position");
    mesh.setupTexCoords(prog | "TexCoord");
    mesh.setupNormals(prog | "Normal");
    mesh.setupBones(prog | "BoneIDs", prog | "Weights");
    oglwrap::UniformSampler(prog, "EnvMap").set(0);
    mesh.setup_diffuse_textures(1);
    mesh.setup_specular_textures(2);
    oglwrap::UniformSampler(prog, "DiffuseTexture").set(1);
    oglwrap::UniformSampler(prog, "SpecularTexture").set(2);

    mesh.addAnimation("models/ayumi_idle.dae", "Stand",
                       oglwrap::AnimFlag::Repeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh.addAnimation("models/ayumi_walk.dae", "Walk",
                       oglwrap::AnimFlag::Repeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh.addAnimation("models/ayumi_run.dae", "Run",
                       oglwrap::AnimFlag::Repeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh.addAnimation("models/ayumi_jump_rise.dae", "JumpRise",
                       oglwrap::AnimFlag::MirroredRepeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh.addAnimation("models/ayumi_jump_fall.dae", "JumpFall",
                       oglwrap::AnimFlag::MirroredRepeat |
                       oglwrap::AnimFlag::Interruptable);

    mesh.addAnimation("models/ayumi_attack.dae", "Attack",
                       oglwrap::AnimFlag::None, 2.0f);

    mesh.setDefaultAnimation("Stand", 0.3f);
    mesh.forceAnimToDefault(0);
  }

  void reshape(glm::mat4 projMat) {
    prog.use();
    projectionMatrix = projMat;
  }

  void render(float time,
              const oglwrap::Camera& cam,
              const oglwrap::CharacterMovement& charmove) {
    prog.use();
    cameraMatrix.set(cam.cameraMatrix());
    modelMatrix.set(charmove.getModelMatrix());
    sunData.set(skybox.getSunData(time));
    skybox.envMap.active(0);
    skybox.envMap.bind();

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      mesh.setCurrentAnimation("Attack", time, 0.3f);
    } else if(charmove.is_jumping()) {
      if(charmove.is_jumping_rise()) {
        mesh.setCurrentAnimation("JumpRise", time, 0.3f);
      } else {
        mesh.setCurrentAnimation("JumpFall", time, 0.3f);
      }
    } else if(charmove.isWalking()) {
      if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        mesh.setCurrentAnimation("Run", time, 0.2f);
      } else {
        mesh.setCurrentAnimation("Walk", time, 0.3f);
      }
    } else {
      mesh.setAnimToDefault(time);
    }

    mesh.boneTransform(time, bones);
    mesh.render();
    skybox.envMap.active(0);
    skybox.envMap.unbind();
  }
};

#endif // LOD_AYUMI_HPP_
