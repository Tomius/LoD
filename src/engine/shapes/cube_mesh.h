// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SHAPES_CUBE_MESH_H_
#define ENGINE_SHAPES_CUBE_MESH_H_

#include "../oglwrap_config.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/shapes/cube.h"

#include "engine/scene.h"
#include "engine/game_object.h"

namespace engine {
namespace shapes {

class CubeMesh : public engine::GameObject {

 private:
  gl::Cube cube_;

  engine::ShaderProgram prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;

  virtual void render() override {
    prog_.use();
    prog_.update();
    const auto& cam = *scene_->camera();
    uCameraMatrix_ = cam.matrix();
    uProjectionMatrix_ = cam.projectionMatrix();
    uModelMatrix_ = transform.matrix();

    cube_.render();
  }
};

}  // namespace engine
}  // namespcae shapes

#endif
