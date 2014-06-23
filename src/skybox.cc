// Copyright (c) 2014, Tamas Csala

#include "./skybox.h"
#include "engine/scene.h"
#include "oglwrap/smart_enums.h"

constexpr float day_duration = 256.0f, day_start = 8.0f;

Skybox::Skybox(engine::Scene* scene)
    : engine::Behaviour(scene)
    , time_(day_start)
    , prog_(scene->shader_manager()->get("skybox.vert"),
            scene->shader_manager()->get("skybox.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix") {
  engine::ShaderFile *sky_fs = scene->shader_manager()->get("sky.frag");
  sky_fs->set_update_func([this](const gl::Program& prog) {
    gl::Uniform<glm::vec3>(prog, "uSunPos") = getSunPos();
  });

  prog_.use().validate();
  cube_.setupPositions(prog_ | "aPosition");
}

glm::vec3 Skybox::getSunPos() const {
  return glm::vec3(0.f, 1.f, 0.f) *
          static_cast<float>(1e10 * sin(time_ * 2 * M_PI / day_duration)) +
         glm::vec3(0.f, 0.f, -1.f) *
          static_cast<float>(1e10 * cos(time_ * 2 * M_PI / day_duration));
}

glm::vec3 Skybox::getLightSourcePos() const {
  glm::vec3 sun_pos = getSunPos();
  return sun_pos.y > 0 ? sun_pos : -sun_pos;
}

void Skybox::update() {
  time_ = scene_->environment_time().current + day_start;
}

void Skybox::render() {
  const engine::Camera& cam = *scene_->camera();

  // We don't need the camera matrix's translation part for the skybox
  const float* f = glm::value_ptr(cam.matrix());
  glm::mat3 cam_rot = glm::mat3(f[0], f[1], f[2],
                                f[4], f[5], f[6],
                                f[8], f[9], f[10]);

  prog_.use();
  prog_.update();
  uCameraMatrix_ = cam_rot;
  uProjectionMatrix_ = cam.projectionMatrix();

  auto depth_test = gl::TemporaryDisable(gl::kDepthTest);

  gl::DepthMask(false);
  cube_.render();
  gl::DepthMask(true);
}
