#include "skybox.hpp"

using namespace oglwrap;
using namespace glm;

Skybox::Skybox()
  : vs_("skybox.vert")
  , fs_("skybox.frag")
  , projectionMatrix_(prog_, "ProjectionMatrix")
  , cameraMatrix_(prog_, "CameraMatrix")
  , sunData_(prog_, "SunData")
  , sky_fs("sky.frag") {

  prog_ << vs_ << fs_ << sky_fs;
  prog_.link().use();

  make_cube_.setupPositions(prog_ | "Position");

  envMap.active(0);
  {
    envMap.bind();
    envMap.minFilter(MinFilter::Linear);
    envMap.magFilter(MagFilter::Linear);
    envMap.wrapS(Wrap::ClampToEdge);
    envMap.wrapT(Wrap::ClampToEdge);
    envMap.wrapR(Wrap::ClampToEdge);

    for(int i = 0; i < 6; i++) {
      char c[2] = {char('0' + i), 0};
      envMap.loadTexture(i, "textures/skybox_" + std::string(c) + ".png");
    }
  }
}

void Skybox::reshape(const glm::mat4& projMat) {
  prog_.use();
  projectionMatrix_.set(projMat);
}

const float day_duration = 256.0f;

glm::vec3 Skybox::SunPos(float time) const {
  return vec3(0.f, 1.f, 0.f) * float(1e10 * sin(time * 2 * M_PI / day_duration)) +
         vec3(0.f, 0.f, -1.f) * float(1e10 * cos(time * 2 * M_PI / day_duration));
}

glm::vec4 Skybox::getSunData(float time) const {

  float daytime = fmod(time, day_duration) / day_duration;

  static int day = true; // day/night
  static float lastSwitch = 0.0f;
  if(0.7f < daytime && daytime < 0.8f && 0.5f < (time - lastSwitch) / day_duration) {
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
    if(dayLerp > 1.0f) {
      dayLerp = 1.0f;
    }
  } else if(!day && 0.0f < dayLerp) {
    dayLerp -= timeDiff / day_Night_Transition_Lenght;
    if(dayLerp < 0.0f) {
      dayLerp = 0.0f;
    }
  }

  return vec4(SunPos(time), dayLerp);
}

void Skybox::render(float time, const glm::mat4& cameraMat) {

  // We don't need the camMat's translation part for the skybox
  const float* f = value_ptr(cameraMat);
  mat3 camRot = mat3(
                  f[0], f[1], f[2],
                  f[4], f[5], f[6],
                  f[8], f[9], f[10]
                );

  prog_.use();
  cameraMatrix_.set(camRot);
  sunData_.set(getSunData(time));

  envMap.active(0);
  envMap.bind();

  // The skybox looks better w/o gamma correction
  bool srgb = gl(IsEnabled(GL_FRAMEBUFFER_SRGB));
  if(srgb) { gl(Disable(GL_FRAMEBUFFER_SRGB)); }
  gl(DepthMask(false));
  make_cube_.render();
  gl(DepthMask(true));
  if(srgb) { gl(Enable(GL_FRAMEBUFFER_SRGB)); }

  envMap.unbind();
}
