// Copyright (c) 2014, Tamas Csala

#include "./after_effects.h"
#include "engine/scene.h"
#include "oglwrap/smart_enums.h"

AfterEffects::AfterEffects(GameObject *parent, Skybox* skybox)
    : Behaviour(parent)
    , prog_(scene_->shader_manager()->get("after_effects.vert"),
            scene_->shader_manager()->get("after_effects.frag"))
    , uScreenSize_(prog_, "uScreenSize")
    , s_uSunPos_(prog_, "s_uSunPos")
    , uZNear_(prog_, "uZNear")
    , uZFar_(prog_, "uZFar")
    , skybox_(skybox) {
  prog_.use();

  gl::UniformSampler(prog_, "uTex").set(0);
  gl::UniformSampler(prog_, "uDepthTex").set(1);
  rect_.setupPositions(prog_ | "aPosition");

  prog_.validate();

  color_tex_.bind();
  color_tex_.upload(gl::kRgb, 1, 1, gl::kRgb, gl::kFloat, nullptr);
  color_tex_.minFilter(gl::kLinearMipmapLinear);
  color_tex_.magFilter(gl::kLinear);
  color_tex_.unbind();

  depth_tex_.bind();
  depth_tex_.upload(gl::kDepthComponent, 1, 1,
                    gl::kDepthComponent, gl::kFloat, nullptr);
  depth_tex_.minFilter(gl::kLinear);
  depth_tex_.magFilter(gl::kLinear);
  depth_tex_.unbind();

  fbo_.bind();
  fbo_.attachTexture(gl::kColorAttachment0, color_tex_);
  fbo_.attachTexture(gl::kDepthAttachment, depth_tex_);
  fbo_.validate();
  fbo_.unbind();
}

void AfterEffects::screenResized(size_t w, size_t h) {
  width_ = w;
  height_ = h;
  prog_.use();
  uScreenSize_ = glm::vec2(w, h);

  color_tex_.bind();
  color_tex_.upload(gl::kRgb, width_, height_, gl::kRgb, gl::kFloat, nullptr);
  color_tex_.unbind();

  depth_tex_.bind();
  depth_tex_.upload(gl::kDepthComponent, width_, height_,
                    gl::kDepthComponent, gl::kFloat, nullptr);
  depth_tex_.unbind();
}

void AfterEffects::update() {
  fbo_.bind();
  gl::Clear().Color().Depth();
}

void AfterEffects::render() {
  fbo_.Unbind();
  color_tex_.bind(0);
  color_tex_.generateMipmap();
  depth_tex_.bind(1);

  prog_.use();
  prog_.update();

  auto cam = scene_->camera();
  uZNear_ = cam->z_near();
  uZFar_ = cam->z_far();
  if (skybox_) {
    glm::vec4 s_sun_pos = cam->projectionMatrix() * glm::vec4(
        glm::mat3(cam->cameraMatrix()) * -glm::normalize(skybox_->getSunPos()), 1.0);
    s_sun_pos /= s_sun_pos.w;
    s_uSunPos_ = glm::vec3(s_sun_pos);
  }

  rect_.render();

  depth_tex_.unbind(1);
  color_tex_.unbind(0);
}
