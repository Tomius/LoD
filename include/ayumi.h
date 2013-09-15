#ifndef HEADER_80C5F5DB68DEED54
#define HEADER_80C5F5DB68DEED54

#pragma once

#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/shapes/animatedMesh.hpp"
#include "oglwrap/utils/camera.hpp"
#include "charmove.hpp"
#include "skybox.h"

class Ayumi {
public:
    oglwrap::AnimatedMesh mesh;
private:
    oglwrap::Program prog;
    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix, modelMatrix, bones;
    oglwrap::LazyUniform<glm::vec4> sunData;
    oglwrap::LazyUniformSampler envMap;

    Skybox& skybox;
public:
    Ayumi(Skybox& skybox)
        : mesh("models/ayumi.dae",
           aiProcessPreset_TargetRealtime_MaxQuality |
           aiProcess_FlipUVs
          )
        , fs("ayumi.frag")
        , projectionMatrix(prog, "ProjectionMatrix")
        , cameraMatrix(prog, "CameraMatrix")
        , modelMatrix(prog, "ModelMatrix")
        , bones(prog, "Bones")
        , sunData(prog, "SunData")
        , envMap(prog, "EnvMap")
        , skybox(skybox) {

        oglwrap::ShaderSource vs_source("ayumi.vert");
        vs_source.insert_macro_value("BONE_ATTRIB_NUM", mesh.get_bone_attrib_num());
        vs_source.insert_macro_value("BONE_NUM", mesh.get_num_bones());
        vs.source(vs_source);

        prog << vs << fs << skybox.sky_fs;
        prog.link().use();

        mesh.setup_positions(prog | "Position");
        mesh.setup_texCoords(prog | "TexCoord");
        mesh.setup_normals(prog | "Normal");
        mesh.setup_bones(prog | "BoneIDs", prog | "Weights");
        oglwrap::UniformSampler(prog, "EnvMap").set(1);

        mesh.add_animation(
            "models/ayumi_idle.dae", "Stand",
            oglwrap::AnimFlag::Repeat | oglwrap::AnimFlag::Interruptable
        );
        mesh.add_animation(
            "models/ayumi_walk.dae", "Walk",
            oglwrap::AnimFlag::Repeat | oglwrap::AnimFlag::Interruptable
        );
        mesh.add_animation(
            "models/ayumi_run.dae", "Run",
            oglwrap::AnimFlag::Repeat | oglwrap::AnimFlag::Interruptable
        );
        mesh.add_animation(
            "models/ayumi_jump_rise.dae", "JumpRise",
            oglwrap::AnimFlag::MirroredRepeat | oglwrap::AnimFlag::Interruptable
        );
        mesh.add_animation(
            "models/ayumi_jump_fall.dae", "JumpFall",
            oglwrap::AnimFlag::MirroredRepeat | oglwrap::AnimFlag::Interruptable
        );
        mesh.set_default_animation("Stand", 0.3f);
        mesh.force_anim_to_default(0);
    }

    void reshape(glm::mat4 projMat) {
        prog.use();
        projectionMatrix = projMat;
    }

    void render(float time, const oglwrap::Camera& cam, const oglwrap::CharacterMovement& charmove) {
        prog.use();
        cameraMatrix.set(cam.cameraMatrix());
        modelMatrix.set(charmove.getModelMatrix());
        sunData.set(skybox.getSunData(time));
        skybox.envMap.active(1);
        skybox.envMap.bind();

        if(charmove.is_jumping()) {
            if(charmove.is_jumping_rise())
                mesh.set_current_animation("JumpRise", time, 0.1f);
            else
                mesh.set_current_animation("JumpFall", time, 0.3f);
        } else if(charmove.isWalking()) {
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                mesh.set_current_animation("Run", time, 0.2f);
            else
                mesh.set_current_animation("Walk", time, 0.3f);
        } else {
            mesh.set_anim_to_default(time);
        }

        oglwrap::Texture2D::active(0);
        mesh.boneTransform(time, bones);
        mesh.render();
        skybox.envMap.active(1);
        skybox.envMap.unbind();
    }
};


#endif // header guard
