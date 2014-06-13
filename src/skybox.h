// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SKYBOX_H_
#define LOD_SKYBOX_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/cube.h"
#include "oglwrap/textures/texture_cube.h"

#include "engine/behaviour.h"

class Skybox : public engine::Behaviour {
 public:
  Skybox();
  virtual ~Skybox() {}

  glm::vec3 getSunPos() const;
  glm::vec3 getLightSourcePos() const;

  virtual void render(const engine::Scene& scene) override;
  virtual void update(const engine::Scene& scene) override;

  const gl::FragmentShader& sky_fs() { return sky_fs_; };

 private:
  float time_;
  gl::Cube cube_;

  gl::VertexShader vs_;
  gl::FragmentShader fs_;
  gl::Program prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;
  gl::LazyUniform<glm::vec3> uSunPos_;

  gl::FragmentShader sky_fs_;
};


#endif  // LOD_SKYBOX_H_
