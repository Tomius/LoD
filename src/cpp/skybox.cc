// Copyright (c) 2014, Tamas Csala

#include "./skybox.h"
#include "oglwrap/smart_enums.h"

const float day_duration = 256.0f, day_start = 0.0f;

Skybox::Skybox(engine::GameObject* parent)
    : engine::GameObject(parent)
    , time_(day_start)
    , cube_({gl::CubeShape::kPosition})
    , prog_(scene_->shader_manager()->get("skybox.vert"),
            scene_->shader_manager()->get("skybox.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix") {
  engine::ShaderFile *sky_fs = scene_->shader_manager()->get("sky.frag");
  sky_fs->set_update_func([this](const gl::Program& prog) {
    gl::Uniform<glm::vec3>(prog, "uSunPos") = getSunPos();
  });

  gl::Use(prog_);
  prog_.validate();
  (prog_ | "aPosition").bindLocation(cube_.kPosition);
}

glm::vec3 Skybox::getSunPos() const {
  return glm::vec3(0.f, 1.f, 0.f) *
          static_cast<float>(sin(time_ * 2 * M_PI / day_duration)) +
         glm::vec3(0.f, 0.f, -1.f) *
          static_cast<float>(cos(time_ * 2 * M_PI / day_duration));
}

glm::vec3 Skybox::getLightSourcePos() const {
  glm::vec3 sun_pos = getSunPos();
  return sun_pos.y > 0 ? sun_pos : -sun_pos;
}

void Skybox::update() {
  time_ = scene_->environment_time().current + day_start;
}

void Skybox::render() {
  auto cam = scene_->camera();

  gl::Use(prog_);
  prog_.update();
  uCameraMatrix_ = glm::mat3(cam->cameraMatrix());
  uProjectionMatrix_ = cam->projectionMatrix();

  gl::TemporaryDisable depth_test{gl::kDepthTest};

  gl::DepthMask(false);
  cube_.render();
  gl::DepthMask(true);
}
