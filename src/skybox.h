// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SKYBOX_H_
#define LOD_SKYBOX_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/cube.h"
#include "oglwrap/textures/texture_cube.h"

#include "engine/gameobject.h"

class Skybox : public engine::GameObject {
  float time_;
  oglwrap::Cube cube_;

  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_;
  oglwrap::LazyUniform<glm::mat3> uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  oglwrap::TextureCube env_map_;
  oglwrap::FragmentShader sky_fs_;
public:
  const oglwrap::TextureCube& env_map;
  const oglwrap::FragmentShader& sky_fs;

  Skybox();
  virtual ~Skybox() {}
  virtual void render(float time, const engine::Camera& cam) override;
  virtual void update(float time) override;

  glm::vec3 getSunPos() const;
  glm::vec4 getSunData() const;
};


#endif // LOD_SKYBOX_H_
