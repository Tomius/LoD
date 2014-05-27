// Copyright (c) 2014, Tamas Csala

#include "./skybox.h"
#include "oglwrap/smart_enums.h"

constexpr float day_duration = 128.0f;
constexpr float day_start = -day_duration/32;

Skybox::Skybox()
    : time_(day_start)
    , vs_("skybox.vert")
    , fs_("skybox.frag")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uSunData_(prog_, "uSunData")
    , sky_fs_("sky.frag") {
  prog_ << vs_ << fs_ << sky_fs_;
  prog_.link().use();

  gl::UniformSampler(prog_, "uEnvMap").set(0);

  prog_.validate();

  cube_.setupPositions(prog_ | "aPosition");

  env_map_.active(0);
  {
    env_map_.bind();
    env_map_.minFilter(gl::kLinear);
    env_map_.magFilter(gl::kLinear);
    env_map_.wrapS(gl::kClampToEdge);
    env_map_.wrapT(gl::kClampToEdge);
    env_map_.wrapP(gl::kClampToEdge);

    for (int i = 0; i < 6; i++) {
      // The cloud map is not in srgb
      env_map_.loadTexture(i, "textures/skybox_" + std::to_string(i) + ".png",
                           "CRGBA");
    }
  }
}

void Skybox::update(float time) {
  time_ = time + day_start;
}

glm::vec3 Skybox::getSunPos() const {
  return glm::vec3(0.f, 1.f, 0.f) *
          static_cast<float>(1e10 * sin(time_ * 2 * M_PI / day_duration)) +
         glm::vec3(0.f, 0.f, -1.f) *
          static_cast<float>(1e10 * cos(time_ * 2 * M_PI / day_duration));
}

glm::vec4 Skybox::getSunData() const {
  float day_time = fmod(time_, day_duration) / day_duration;

  static bool day = true;  // day/night
  static float last_switch = 0.0f;
  if (0.7f < day_time && day_time < 0.8f &&
      0.5f < (time_ - last_switch) / day_duration) {
    day = !day;
    last_switch = time_;
  }

  static float day_lerp = 1.0f;
  // The transition happens between the day_times of 0.7 and 0.8
  const float day_night_transition_lenght = 0.1f * day_duration;

  static float last_time = time_;
  float time_diff = time_ - last_time;
  last_time = time_;

  if (day && day_lerp < 1.0f) {
    day_lerp += time_diff / day_night_transition_lenght;
    if (day_lerp > 1.0f) {
      day_lerp = 1.0f;
    }
  } else if (!day && 0.0f < day_lerp) {
    day_lerp -= time_diff / day_night_transition_lenght;
    if (day_lerp < 0.0f) {
      day_lerp = 0.0f;
    }
  }

  return glm::vec4(getSunPos(), day_lerp);
}

void Skybox::render(float time, const engine::Camera& cam) {
  // We don't need the camera matrix's translation part for the skybox
  const float* f = glm::value_ptr(cam.matrix());
  glm::mat3 cam_rot = glm::mat3(f[0], f[1], f[2],
                                f[4], f[5], f[6],
                                f[8], f[9], f[10]);

  prog_.use();
  uCameraMatrix_ = cam_rot;
  uProjectionMatrix_ = cam.projectionMatrix();
  uSunData_ = getSunData();

  env_map_.active(0);
  env_map_.bind();
  gl::DepthMask(false);

  cube_.render();

  gl::DepthMask(true);
  env_map_.active(0);
  env_map_.unbind();
}
