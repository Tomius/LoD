// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SKYBOX_H_
#define LOD_SKYBOX_H_

#include "engine/oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/cube_shape.h"
#include "oglwrap/textures/texture_cube.h"

#include "engine/scene.h"
#include "engine/behaviour.h"

class Skybox : public engine::Behaviour {
 public:
  explicit Skybox(engine::GameObject* parent);
  virtual ~Skybox() {}

  glm::vec3 getSunPos() const;
  glm::vec3 getLightSourcePos() const;

  virtual void render() override;
  virtual void update() override;

 private:
  float time_;
  gl::CubeShape cube_;

  engine::ShaderProgram prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;
};


#endif  // LOD_SKYBOX_H_
