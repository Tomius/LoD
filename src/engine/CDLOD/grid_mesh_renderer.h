// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_RENDERER_H_
#define ENGINE_CDLOD_GRID_MESH_RENDERER_H_

#include "grid_mesh.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"

#include "../gameobject.h"

namespace engine {

class GridMeshRenderer {
  GridMesh mesh_;
  oglwrap::Program prog_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec3> uCamPos_;

 public:
  static constexpr GLubyte max_dim = 128;
  GLubyte dimension_;

  GridMeshRenderer(GLubyte dimension = 128)
      : uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , uCamPos_(prog_, "uCamPos")
      , dimension_(dimension) {
    oglwrap::VertexShader vs{"grid_mesh_renderer.vert"};
    oglwrap::FragmentShader fs{"grid_mesh_renderer.frag"};

    prog_ << vs << fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "aPosition", dimension);
    mesh_.setupRenderData(prog_ | "aRenderData");
    prog_.validate();
  }

  void set_dimension(GLubyte dimension) {
    dimension_ = dimension;
    mesh_.setupPositions(prog_ | "aPosition", dimension);
  }

  void clearRenderList() {
    mesh_.clearRenderList();
  }

  void addToRenderList(glm::vec2 offset, float scale, int level,
                       bool tl = true, bool tr = true,
                       bool bl = true, bool br = true) {
    scale *= (max_dim/dimension_);
    mesh_.addToRenderList(glm::vec4(offset, scale, level), tl, tr, bl, br);
  }

  void render(const Camera& cam) {
    using oglwrap::Context;
    using oglwrap::Capability;
    using oglwrap::PolyMode;
    using oglwrap::FaceOrientation;

    prog_.use();
    uCamPos_ = cam.pos();
    uCameraMatrix_ = cam.matrix();
    uProjectionMatrix_ = cam.projectionMatrix();

    Context::FrontFace(FaceOrientation::CCW);
    Context::TemporaryEnable cullface(Capability::CullFace);

    Context::PolygonMode(PolyMode::Line);
    mesh_.render();
    Context::PolygonMode(PolyMode::Fill);

    prog_.unuse();
  }
};

}

#endif
