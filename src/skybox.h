// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SKYBOX_H_
#define LOD_SKYBOX_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/cube.h"
#include "oglwrap/textures/texture_cube.h"

#include "engine/game_object.h"

class Skybox : public engine::GameObject {
 public:
  Skybox();
  virtual ~Skybox() {}
  virtual void render(float time, const engine::Camera& cam) override;
  virtual void update(float time) override;

  glm::vec3 getSunPos() const;
  glm::vec4 getSunData() const;

  const gl::TextureCube& env_map() { return env_map_; };
  const gl::FragmentShader& sky_fs() { return sky_fs_; };

 private:
  float time_;
  gl::Cube cube_;

  gl::VertexShader vs_;
  gl::FragmentShader fs_;
  gl::Program prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;
  gl::LazyUniform<glm::vec4> uSunData_;

  gl::TextureCube env_map_;
  gl::FragmentShader sky_fs_;
};


#endif  // LOD_SKYBOX_H_
