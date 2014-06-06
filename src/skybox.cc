// Copyright (c) 2014, Tamas Csala

#include "./skybox.h"
#include "engine/scene.h"
#include "oglwrap/smart_enums.h"

constexpr float day_duration = 256.0f, day_start = 0.0f;

Skybox::Skybox()
    : time_(day_start)
    , vs_("skybox.vert")
    , fs_("skybox.frag")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uSunPos_(prog_, "uSunPos")
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

glm::vec3 Skybox::getSunPos() const {
  return glm::vec3(0.f, 1.f, 0.f) *
          static_cast<float>(1e10 * sin(time_ * 2 * M_PI / day_duration)) +
         glm::vec3(0.f, 0.f, -1.f) *
          static_cast<float>(1e10 * cos(time_ * 2 * M_PI / day_duration));
}

void Skybox::update(const engine::Scene& scene) {
  time_ = scene.environment_time().current + day_start;
}

void Skybox::render(const engine::Scene& scene) {
  const engine::Camera& cam = *scene.camera();

  // We don't need the camera matrix's translation part for the skybox
  const float* f = glm::value_ptr(cam.matrix());
  glm::mat3 cam_rot = glm::mat3(f[0], f[1], f[2],
                                f[4], f[5], f[6],
                                f[8], f[9], f[10]);

  prog_.use();
  uCameraMatrix_ = cam_rot;
  uProjectionMatrix_ = cam.projectionMatrix();
  uSunPos_ = getSunPos();

  auto depth_test = gl::TemporaryDisable(gl::kDepthTest);

  env_map_.active(0);
  env_map_.bind();
  gl::DepthMask(false);

  cube_.render();

  gl::DepthMask(true);
  env_map_.active(0);
  env_map_.unbind();
}
