// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_DEBUG_DEBUG_SHAPE_H_
#define ENGINE_DEBUG_DEBUG_SHAPE_H_

#include "../oglwrap_config.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/shapes/cube_shape.h"
#include "../../oglwrap/shapes/sphere_shape.h"

#include "../scene.h"
#include "../game_object.h"

namespace engine {
namespace debug {

template <typename Shape_t>
class DebugShape : public GameObject {
 public:
  DebugShape(GameObject* parent, const glm::vec3& color = glm::vec3());

  glm::vec3 color() { return color_; }
  void set_color(const glm::vec3& color) { color_ = color; }

 private:
  static Shape_t *shape_;

  static engine::ShaderProgram *prog_;
  static gl::LazyUniform<glm::mat4> *uProjectionMatrix_, *uCameraMatrix_,
                                    *uModelMatrix_;
  static gl::LazyUniform<glm::vec3> *uColor_;
  glm::vec3 color_;

  virtual void render() override;
};

using Cube = DebugShape<gl::CubeShape>;
using Sphere = DebugShape<gl::SphereShape>;

}  // namespcae debug
}  // namespace engine

#include "./debug_shape-inl.h"

#endif
