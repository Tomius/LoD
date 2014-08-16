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
  gl::Use(prog_);

  gl::UniformSampler(prog_, "uTex").set(layout_.add(color_tex_));
  gl::UniformSampler(prog_, "uDepthTex").set(layout_.add(depth_tex_));
  (prog_ | "aPosition").bindLocation(rect_.kPosition);

  prog_.validate();

  {
    gl::BoundTexture2D tex{color_tex_};
    tex.upload(gl::kRgb, 1, 1, gl::kRgb, gl::kFloat, nullptr);
    tex.minFilter(gl::kLinearMipmapLinear);
    tex.magFilter(gl::kLinear);
  }

  {
    gl::BoundTexture2D tex{depth_tex_};
    tex.upload(gl::kDepthComponent, 1, 1,
                    gl::kDepthComponent, gl::kFloat, nullptr);
    tex.minFilter(gl::kLinear);
    tex.magFilter(gl::kLinear);
  }

  gl::BoundFramebuffer fbo{fbo_};
  fbo.attachTexture(gl::kColorAttachment0, color_tex_);
  fbo.attachTexture(gl::kDepthAttachment, depth_tex_);
  fbo.validate();
}

void AfterEffects::screenResized(size_t w, size_t h) {
  width_ = w;
  height_ = h;
  gl::Use(prog_);
  uScreenSize_ = glm::vec2(w, h);

  gl::BoundTexture2D{color_tex_}.upload(gl::kRgb, width_, height_,
                                        gl::kRgb, gl::kFloat, nullptr);

  gl::BoundTexture2D{depth_tex_}.upload(gl::kDepthComponent, width_, height_,
                                        gl::kDepthComponent, gl::kFloat,
                                        nullptr);
}

void AfterEffects::update() {
  fbo_binding_ = new gl::BoundFramebuffer{fbo_};
  gl::Clear().Color().Depth();
}

void AfterEffects::render() {
  delete fbo_binding_;

  gl::BoundTexture2D{color_tex_}.generateMipmap();

  layout_.bind();

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

  layout_.unbind();
}
