// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_RENDERER_H_
#define ENGINE_CDLOD_GRID_MESH_RENDERER_H_

#include "grid_mesh.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"

#include "../gameobject.h"


class GridMeshRenderer : public engine::GameObject {
  GridMesh mesh_;
  oglwrap::Program prog_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec3> uCamPos_;
  oglwrap::LazyUniform<glm::vec2> uOffset_;
  oglwrap::LazyUniform<float> uScale_;
  oglwrap::LazyUniform<int> uLevel_;

 public:
  static constexpr GLubyte max_dim = 128;
  GLubyte dimension_;

  GridMeshRenderer(GLubyte dimension = 128)
      : uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , uCamPos_(prog_, "uCamPos")
      , uOffset_(prog_, "uOffset")
      , uScale_(prog_, "uScale")
      , uLevel_(prog_, "uLevel")
      , dimension_(dimension) {
    oglwrap::VertexShader vs{"grid_mesh_renderer.vert"};
    oglwrap::FragmentShader fs{"grid_mesh_renderer.frag"};

    prog_ << vs << fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "aPosition", dimension);
    prog_.validate();
  }

  void set_dimension(GLubyte dimension) {
    dimension_ = dimension;
    mesh_.setupPositions(prog_ | "aPosition", dimension);
  }

  void render(const engine::Camera& cam, glm::vec2 offset, float scale, int level,
              bool TL = true, bool TR = true, bool BL = true, bool BR = true) {
    using oglwrap::Context;
    using oglwrap::Capability;
    using oglwrap::PolyMode;
    using oglwrap::FaceOrientation;

    prog_.use();
    uCameraMatrix_ = cam.matrix();
    uProjectionMatrix_ = cam.projectionMatrix();
    uOffset_ = offset;
    uScale_ = scale * (max_dim/dimension_);
    uLevel_ = level;
    uCamPos_ = cam.pos();

    Context::FrontFace(FaceOrientation::CCW);
    Context::TemporaryEnable cullface(Capability::CullFace);

    Context::PolygonMode(PolyMode::Line);
    mesh_.render(TL, TR, BL, BR);
    Context::PolygonMode(PolyMode::Fill);

    prog_.unuse();
  }

  virtual void render(float time, const engine::Camera& cam) override {
    render(cam, glm::vec2(0, 0), 1, 0);
  }

};

#endif
