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
      : GameObject(parent), color_(color) {
    if (!cube_) {
      cube_ = new gl::Cube{{gl::Cube::kPosition, gl::Cube::kNormal}};
    }
    if (!prog_) {
      prog_ = new engine::ShaderProgram{
                  scene_->shader_manager()->get("engine/simple_shape.vert"),
                  scene_->shader_manager()->get("engine/simple_shape.frag")};
      (*prog_ | "aPosition").bindLocation(cube_->kPosition);
      (*prog_ | "aNormal").bindLocation(cube_->kNormal);
      uProjectionMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uProjectionMatrix"};
      uCameraMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uCameraMatrix"};
      uModelMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uModelMatrix"};
      uColor_ = new gl::LazyUniform<glm::vec3>{*prog_, "uColor"};
    }
  }

  glm::vec3 color() { return color_; }
  void set_color(const glm::vec3& color) { color_ = color; }

 private:
  static gl::Cube *cube_;

  static engine::ShaderProgram *prog_;
  static gl::LazyUniform<glm::mat4> *uProjectionMatrix_, *uCameraMatrix_, *uModelMatrix_;
  static gl::LazyUniform<glm::vec3> *uColor_;
  glm::vec3 color_;

  virtual void render() override {
    prog_->use();
    prog_->update();
    const auto& cam = *scene_->camera();
    uCameraMatrix_->set(cam.cameraMatrix());
    uProjectionMatrix_->set(cam.projectionMatrix());
    uModelMatrix_->set(transform()->matrix());
    uColor_->set(color_);

    gl::FrontFace(cube_->faceWinding());
    gl::TemporaryEnable cullface{gl::kCullFace};
    cube_->render();
  }
};

}  // namespace engine
}  // namespcae shapes

#endif
