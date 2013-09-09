#ifndef HEADER_80C5F5DB68DEED54
#define HEADER_80C5F5DB68DEED54

#pragma once

#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/shapes/animatedMesh.hpp"
#include "oglwrap/utils/camera.hpp"
#include "charmove.hpp"

class Ayumi {
public:
    oglwrap::AnimatedMesh mesh;
private:
    oglwrap::Program prog;
    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix, modelMatrix, bones;
public:
    Ayumi()
        : mesh("models/ayumi.dae",
           aiProcessPreset_TargetRealtime_MaxQuality |
           aiProcess_FlipUVs
          )
        , fs("ayumi.frag")
        , projectionMatrix(prog, "ProjectionMatrix")
        , cameraMatrix(prog, "CameraMatrix")
        , modelMatrix(prog, "ModelMatrix")
        , bones(prog, "Bones") {

        oglwrap::ShaderSource vs_source("ayumi.vert");
        vs_source.insert_macro_value("BONE_ATTRIB_NUM", mesh.get_bone_attrib_num());
        vs_source.insert_macro_value("BONE_NUM", mesh.get_num_bones());
        vs.source(vs_source);

        prog << vs << fs;
        prog.link().use();

        mesh.setup_positions(prog | "Position");
        mesh.setup_texCoords(prog | "TexCoord");
        mesh.setup_normals(prog | "Normal");
        mesh.setup_bones(prog | "BoneIDs", prog | "Weights");

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
            "models/ayumi_jump.dae", "Jump",
            oglwrap::AnimFlag::None
        );
        mesh.set_default_animation("Stand", 0.2f);
        mesh.force_anim_to_default(0);
    }

    void reshape(glm::mat4 projMat) {
        prog.use();
        projectionMatrix = projMat;
    }

    void render(float time, const oglwrap::Camera& cam, const oglwrap::CharacterMovement& charmove) {
        prog.use();
        oglwrap::Texture2D::active(0);
        cameraMatrix.set(cam.cameraMatrix());
        modelMatrix.set(charmove.getModelMatrix());

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            mesh.set_current_animation("Jump", time, 0.2f);
        } else if(charmove.isWalking()) {
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                mesh.set_current_animation("Run", time, 0.2f);
            else
                mesh.set_current_animation("Walk", time, 0.3f);
        } else {
            mesh.set_anim_to_default(time);
        }

        mesh.boneTransform(time, bones);
        mesh.render();
    }
};


#endif // header guard
