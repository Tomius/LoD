// Copyright (c) 2014, Tamas Csala

#include "bloom.h"
#include "oglwrap/smart_enums.h"

namespace gl = oglwrap;
using glEnum = oglwrap::SmartEnums;

BloomEffect::BloomEffect()
    : vs_("bloom.vert")
    , fs_("bloom.frag")
    , uScreenSize_(prog_, "uScreenSize") {
  prog_ << vs_ << fs_;
  prog_.link().use();

  gl::UniformSampler(prog_, "uTex").set(0);

  prog_.validate();

  rect_.setupPositions(prog_ | "aPosition");

  tex_.active(0);
  tex_.bind();
  tex_.minFilter(glEnum::Linear);
  tex_.magFilter(glEnum::Linear);
  tex_.unbind();
}

void BloomEffect::screenResized(size_t w, size_t h) {
  width_ = w;
  height_ = h;
  prog_.use();
  uScreenSize_ = glm::vec2(w, h);

  tex_.active(0);
  tex_.bind();
  tex_.upload(glEnum::Rgb, width_, height_, glEnum::Rgb, glEnum::Float, nullptr);
  tex_.unbind();
}

void BloomEffect::render(float, const engine::Camera&) {
  // Copy the backbuffer to the texture that our shader can fetch.
  tex_.active(0);
  tex_.bind();
  tex_.copy(glEnum::Rgb, 0, 0, width_, height_);

  gl::Clear().Color().Depth();

  prog_.use();
  rect_.render();

  tex_.unbind();
}
