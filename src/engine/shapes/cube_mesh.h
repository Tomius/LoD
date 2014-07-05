// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SHAPES_CUBE_MESH_H_
#define ENGINE_SHAPES_CUBE_MESH_H_

#include "../oglwrap_config.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/shapes/cube.h"

#include "../scene.h"
#include "../game_object.h"

namespace engine {
namespace shapes {

class CubeMesh : public GameObject {
 public:
  CubeMesh(GameObject* parent, const glm::vec3& color = glm::vec3())
      : GameObject(parent)
      , prog_(scene_->shader_manager()->get("engine/simple_shape.vert"),
              scene_->shader_manager()->get("engine/simple_shape.frag"))
      , uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , uModelMatrix_(prog_, "uModelMatrix")
      , uColor_(prog_, "uColor") {
    prog_.use();
    cube_.setupPositions(prog_ | "aPosition");
    cube_.setupNormals(prog_ | "aNormal");
    uColor_ = color;
  }

  glm::vec3 color() { return uColor_; }
  void set_color(const glm::vec3& color) { uColor_ = color; }

 private:
  gl::Cube cube_;

  engine::ShaderProgram prog_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_;
  gl::LazyUniform<glm::vec3> uColor_;

  virtual void render() override {
    prog_.use();
    prog_.update();
    const auto& cam = *scene_->camera();
    uCameraMatrix_ = cam.matrix();
    uProjectionMatrix_ = cam.projectionMatrix();
    uModelMatrix_ = transform()->matrix();

    cube_.render();
  }
};

}  // namespace engine
}  // namespcae shapes

#endif
