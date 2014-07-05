// Copyright (c) 2014, Tamas Csala

#include "./bloom.h"
#include "engine/scene.h"
#include "oglwrap/smart_enums.h"

BloomEffect::BloomEffect(GameObject *parent)
    : Behaviour(parent)
    , prog_(scene_->shader_manager()->get("bloom.vert"),
            scene_->shader_manager()->get("bloom.frag"))
    , uScreenSize_(prog_, "uScreenSize")
    , uZNear_(prog_, "uZNear")
    , uZFar_(prog_, "uZFar") {
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

void BloomEffect::screenResized(size_t w, size_t h) {
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

void BloomEffect::update() {
  fbo_.bind();
  gl::Clear().Color().Depth();
}

void BloomEffect::render() {
  fbo_.Unbind();
  color_tex_.bind(0);
  color_tex_.generateMipmap();
  depth_tex_.bind(1);

  prog_.use();
  auto cam = scene_->camera();
  uZNear_ = cam->z_near();
  uZFar_ = cam->z_far();

  rect_.render();

  depth_tex_.unbind(1);
  color_tex_.unbind(0);
}
