// Copyright (c) 2014, Tamas Csala

#include "./after_effects.h"
#include "engine/scene.h"
#include "engine/misc.h"
#include "oglwrap/smart_enums.h"

AfterEffects::AfterEffects(GameObject *parent, Skybox* skybox)
    : Behaviour(parent)
    , uScreenSize_(prog_, "uScreenSize")
    , s_uSunPos_(prog_, "s_uSunPos")
    , uZNear_(prog_, "uZNear")
    , uZFar_(prog_, "uZFar")
    , skybox_(skybox) {
  engine::ShaderFile *vs = scene_->shader_manager()->get("after_effects.vert");
  engine::ShaderFile *fs = scene_->shader_manager()->get("after_effects_dof.frag");
  if (fs->state() != gl::Shader::kCompileSuccessful) {
    // intel doesn't support textureLoD, so no DoF :(
    fs = scene_->shader_manager()->get("after_effects_no_dof.frag");
  }

  prog_.attachShaders(vs, fs).link();
  gl::Use(prog_);

  gl::UniformSampler(prog_, "uTex").set(0);
  gl::UniformSampler(prog_, "uDepthTex").set(1);
  (prog_ | "aPosition").bindLocation(rect_.kPosition);

  prog_.validate();

  gl::Bind(color_tex_);
  color_tex_.upload(gl::kRgb, 1, 1, gl::kRgb, gl::kFloat, nullptr);
  color_tex_.minFilter(gl::kLinearMipmapLinear);
  color_tex_.magFilter(gl::kLinear);
  gl::Unbind(color_tex_);

  gl::Bind(depth_tex_);
  depth_tex_.upload(gl::kDepthComponent, 1, 1,
                    gl::kDepthComponent, gl::kFloat, nullptr);
  depth_tex_.minFilter(gl::kLinear);
  depth_tex_.magFilter(gl::kLinear);
  gl::Unbind(depth_tex_);

  gl::Bind(fbo_);
  fbo_.attachTexture(gl::kColorAttachment0, color_tex_);
  fbo_.attachTexture(gl::kDepthAttachment, depth_tex_);
  fbo_.validate();
  gl::Unbind(fbo_);
}

void AfterEffects::screenResized(size_t w, size_t h) {
  width_ = w;
  height_ = h;
  gl::Use(prog_);
  uScreenSize_ = glm::vec2(w, h);

  gl::Bind(color_tex_);
  color_tex_.upload(gl::kRgb, width_, height_, gl::kRgb, gl::kFloat, nullptr);
  gl::Unbind(color_tex_);

  gl::Bind(depth_tex_);
  depth_tex_.upload(gl::kDepthComponent, width_, height_,
                    gl::kDepthComponent, gl::kFloat, nullptr);
  gl::Unbind(depth_tex_);
}

void AfterEffects::update() {
  gl::Bind(fbo_);
  gl::Clear().Color().Depth();
}

void AfterEffects::render() {
  gl::Unbind(gl::kFramebuffer);

  gl::BindToTexUnit(color_tex_, 0);
  color_tex_.generateMipmap();
  gl::BindToTexUnit(depth_tex_, 1);

  gl::Use(prog_);
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

  gl::UnbindFromTexUnit(depth_tex_, 1);
  gl::UnbindFromTexUnit(color_tex_, 0);
}
