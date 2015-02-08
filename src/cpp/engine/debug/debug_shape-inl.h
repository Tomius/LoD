// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_DEBUG_DEBUG_SHAPE_INL_H_
#define ENGINE_DEBUG_DEBUG_SHAPE_INL_H_

#include "./debug_shape.h"

namespace engine {
namespace debug {

template<typename Shape_t>
Shape_t *DebugShape<Shape_t>::shape_ = nullptr;
template<typename Shape_t>
engine::ShaderProgram *DebugShape<Shape_t>::prog_ = nullptr;
template<typename Shape_t>
gl::LazyUniform<glm::mat4> *DebugShape<Shape_t>::uProjectionMatrix_;
template<typename Shape_t>
gl::LazyUniform<glm::mat4> *DebugShape<Shape_t>::uCameraMatrix_;
template<typename Shape_t>
gl::LazyUniform<glm::mat4> *DebugShape<Shape_t>::uModelMatrix_;
template<typename Shape_t>
gl::LazyUniform<glm::vec3> *DebugShape<Shape_t>::uColor_;

template<typename Shape_t>
DebugShape<Shape_t>::DebugShape(GameObject* parent, const glm::vec3& color)
      : GameObject(parent), color_(color) {
  if (!shape_) {
    shape_ = new Shape_t{{Shape_t::kPosition, Shape_t::kNormal}};
  }
  if (!prog_) {
    prog_ = new engine::ShaderProgram{
                scene_->shader_manager()->get("engine/simple_shape.vert"),
                scene_->shader_manager()->get("engine/simple_shape.frag")};
    (*prog_ | "aPosition").bindLocation(shape_->kPosition);
    (*prog_ | "aNormal").bindLocation(shape_->kNormal);
    uProjectionMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uProjectionMatrix"};
    uCameraMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uCameraMatrix"};
    uModelMatrix_ = new gl::LazyUniform<glm::mat4>{*prog_, "uModelMatrix"};
    uColor_ = new gl::LazyUniform<glm::vec3>{*prog_, "uColor"};
  }
}

template<typename Shape_t>
void DebugShape<Shape_t>::render() {
  gl::Use(*prog_);
  const auto& cam = *scene_->camera();
  uCameraMatrix_->set(cam.cameraMatrix());
  uProjectionMatrix_->set(cam.projectionMatrix());
  uModelMatrix_->set(transform()->matrix());
  uColor_->set(color_);

  gl::FrontFace(shape_->faceWinding());
  gl::TemporaryEnable cullface{gl::kCullFace};
  shape_->render();
}

}  // namespace debug
}  // namespace engine

#endif
