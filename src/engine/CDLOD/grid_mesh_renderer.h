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

    oglwrap::VertexShader vs;

    #ifdef glVertexAttribDivisor
      if (glVertexAttribDivisor)
        vs.sourceFile("grid_mesh_renderer.vert");
      else
    #endif
        vs.sourceFile("grid_mesh_renderer_no_vertex_attrib_divisor.vert");

    oglwrap::FragmentShader fs{"grid_mesh_renderer.frag"};

    prog_ << vs << fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "aPosition", dimension);

    #ifdef glVertexAttribDivisor
      if (glVertexAttribDivisor) {
        mesh_.setupRenderData(prog_ | "aRenderData");
      }
    #endif

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
    #ifdef glVertexAttribDivisor
      if (!glVertexAttribDivisor)
    #endif
    {
      uOffset_ = offset;
      uScale_ = scale;
      uLevel_ = level;
    }

    mesh_.addToRenderList(glm::vec4(offset, scale, level), tl, tr, bl, br);
  }

  void setup_render(const Camera& cam) {
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
  }

  void render() {
    using oglwrap::Context;
    using oglwrap::PolyMode;

    mesh_.render();
    Context::PolygonMode(PolyMode::Fill);
    prog_.unuse();
  }
};

}

#endif
