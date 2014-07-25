// Copyright (c) 2014, Tamas Csala

#ifndef LOD_AFTER_EFFECTS_H_
#define LOD_AFTER_EFFECTS_H_

#include "engine/oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/framebuffer.h"
#include "oglwrap/shapes/rectangle_shape.h"
#include "oglwrap/textures/texture_2D.h"

#include "engine/behaviour.h"
#include "engine/shader_manager.h"

#include "./skybox.h"

class AfterEffects : public engine::Behaviour {
 public:
  explicit AfterEffects(GameObject *parent, Skybox* skybox);

  gl::Framebuffer* fbo() { return &fbo_; }

 private:
  engine::ShaderProgram prog_;

  gl::Framebuffer fbo_;
  gl::Texture2D color_tex_, depth_tex_;
  gl::LazyUniform<glm::vec2> uScreenSize_;
  gl::LazyUniform<glm::vec3> s_uSunPos_;
  gl::LazyUniform<float> uZNear_, uZFar_;
  gl::RectangleShape rect_;

  GLuint width_, height_;

  Skybox* skybox_;

  virtual void screenResized(size_t width, size_t height) override;
  virtual void update() override;
  virtual void render() override;
};


#endif  // LOD_BLOOM_H_
