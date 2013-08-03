#include "skybox.h"
#include "misc.h"
#include <oglplus/shapes/wicker_torus.hpp>
#include <oglplus/shapes/sky_box.hpp>

#include <oglplus/bound/texture.hpp>
#include <oglplus/images/load.hpp>
#include <oglplus/opt/list_init.hpp>

using namespace oglplus;

Skybox::Skybox()
    : skybox(List("Position").Get(), shapes::SkyBox())
    , vs(ObjectDesc("Skybox"), File2Str("skybox.vert"))
    , fs(ObjectDesc("Skybox"), File2Str("skybox.frag"))
    , prog(ObjectDesc("Skybox"))
    , projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , sunData(prog, "SunData")
    , sky_fs(ObjectDesc("Sky"), File2Str("sky.frag")) {

    prog << vs << fs << sky_fs;
    prog.Link();
    skybox.UseInProgram(prog);

    {
        auto btex = oglplus::Bind(envMap, Texture::Target::CubeMap);
        btex.MinFilter(TextureMinFilter::Linear);
        btex.MagFilter(TextureMagFilter::Linear);
        btex.WrapS(TextureWrap::ClampToEdge);
        btex.WrapT(TextureWrap::ClampToEdge);
        btex.WrapR(TextureWrap::ClampToEdge);

        for(int i = 0; i < 6; i++) {
            char c[2] = {char('0' + i), 0};
            LoadTexture("textures/skybox_" + std::string(c) + ".png", Texture::CubeMapFace(i));
        }
    }
}

// ~~~~~~<{ Reshape }>~~~~~~
void Skybox::Reshape(const CamMatrixf& projMat) {
    projectionMatrix.Set(projMat);
}

oglplus::Vec4f Skybox::getSunData(float time) const {
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

    Vec3f sun =
        Vec3f(0.000, 1.000, 0.000) * 1e10 * SineWave(time / day_duration) +
        Vec3f(0.000, 0.000, -1.000) * 1e10 * CosineWave(time / day_duration);

    return Vec4f(sun, dayLerp);
}

#include <iostream>
void Skybox::Render(float time, const CamMatrixf& cameraMat) {
    // We don't need the camMat's translation part for the skybox
    const float* f = Mat3f(cameraMat).Data();
    Mat4f camRot = Mat4f(
        f[0], f[1], f[2], 0.0f,
        f[3], f[4], f[5], 0.0f,
        f[6], f[7], f[8], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    skybox.Use();
    prog.Use();
    cameraMatrix.Set(camRot);
    sunData.Set(getSunData(time));

    // The skybox looks better w/o gamma correction
    bool srgb = gl.IsEnabled(oglplus::Capability::FramebufferSrgb);
    if(srgb) { gl.Disable(oglplus::Capability::FramebufferSrgb); }
    gl.DepthMask(false);
    skybox.Draw();
    gl.DepthMask(true);
    if(srgb) { gl.Enable(oglplus::Capability::FramebufferSrgb); }
}
