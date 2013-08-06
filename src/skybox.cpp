#include <GL/glew.h>
#include "skybox.h"

using namespace oglwrap;
using namespace glm;

Skybox::Skybox()
    : vs("skybox.vert")
    , fs("skybox.frag")
    , projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , sunData(prog, "SunData")
    , sky_fs("sky.frag") {

    prog << vs << fs << sky_fs;
    prog.Link();

    vao.Bind();

    positions.Bind();
    {
         std::vector<vec3> corners;
         for(int i = 0; i < 8; i++){
            float x = i%2 < 1 ? -1.0f : +1.0f;
            float y = i%4 < 2 ? -1.0f : +1.0f;
            float z = i%8 < 4 ? -1.0f : +1.0f;
            corners.push_back(vec3(x, y, z));
         }
         positions.Data(corners);
         VertexAttribArray(prog, "Position").Pointer(3).Enable();
    }

    #define RESTART 0xFF
    indices.Bind();
    {
        GLubyte indicesArray[] = {
            1, 3, 5, 7, RESTART,
            4, 6, 0, 2, RESTART,
            2, 6, 3, 7, RESTART,
            4, 0, 5, 1, RESTART,
            5, 7, 4, 6, RESTART,
            0, 2, 1, 3, RESTART
        };
        indices.Data(sizeof(indicesArray), indicesArray);
    }

    {
        envMap.Bind();
        envMap.MinFilter(MinF::Linear);
        envMap.MagFilter(MagF::Linear);
        envMap.WrapS(Wrap::ClampToEdge);
        envMap.WrapT(Wrap::ClampToEdge);
        envMap.WrapR(Wrap::ClampToEdge);

       for(int i = 0; i < 6; i++) {
            char c[2] = {char('0' + i), 0};
            envMap.LoadTexture(i, "textures/skybox_" + std::string(c) + ".png");
        }
    }
}

// ~~~~~~<{ Reshape }>~~~~~~
void Skybox::Reshape(const glm::mat4& projMat) {
    projectionMatrix.Set(projMat);
}

glm::vec4 Skybox::getSunData(float time) const {
    const float day_duration = 256.0f;
    float daytime = fmod(time, day_duration) / day_duration;

    static int day = true; // day/night
    static float lastSwitch = 0.0f;
    if(0.7f < daytime && daytime < 0.8f && 0.5f < (time - lastSwitch) / day_duration){
        day = !day;
        lastSwitch = time;
    }

    static float dayLerp = 1.0f;
    // The transition happens between the daytimes of 0.7 and 0.8
    const float day_Night_Transition_Lenght = 0.1f * day_duration;

    static float lastTime = time;
    float timeDiff = time - lastTime;
    lastTime = time;

    if(day && dayLerp < 1.0f) {
        dayLerp += timeDiff / day_Night_Transition_Lenght;
        if(dayLerp > 1.0f)
            dayLerp = 1.0f;
    } else if (!day && 0.0f < dayLerp) {
        dayLerp -= timeDiff / day_Night_Transition_Lenght;
        if(dayLerp < 0.0f)
            dayLerp = 0.0f;
    }

    vec3 sun =
        vec3(0.f, 1.f, 0.f) * float(1e10 * sin(time * 2 * M_PI / day_duration)) +
        vec3(0.f, 0.f, -1.f) * float(1e10 * cos(time * 2 * M_PI / day_duration));

    return vec4(sun, dayLerp);
}

void Skybox::Render(float time, const glm::mat4& cameraMat) {
    // We don't need the camMat's translation part for the skybox
    const float* f = value_ptr(mat3(cameraMat));
    mat4 camRot = mat4(
        f[0], f[1], f[2], 0.0f,
        f[3], f[4], f[5], 0.0f,
        f[6], f[7], f[8], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    prog.Use();
    cameraMatrix.Set(camRot);
    sunData.Set(getSunData(time));

    // The skybox looks better w/o gamma correction
    bool srgb = glIsEnabled(GL_FRAMEBUFFER_SRGB);
    if(srgb) { glDisable(GL_FRAMEBUFFER_SRGB); }
    glDepthMask(false);
    vao.Bind();
    glDrawElements(GL_TRIANGLE_STRIP, indices.Size(), DataType::UnsignedByte, nullptr);
    glDepthMask(true);
    if(srgb) { glEnable(GL_FRAMEBUFFER_SRGB); }
}
