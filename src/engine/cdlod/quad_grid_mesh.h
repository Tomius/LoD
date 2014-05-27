// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_GRID_MESH_H_
#define ENGINE_CDLOD_QUAD_GRID_MESH_H_

#include "grid_mesh.h"

namespace engine {

namespace cdlod {

// Makes up four, separately renderable GridMeshes.
class QuadGridMesh {
  GridMesh mesh_;

 public:
  // Specify the size of the 4 subquads together, not the size of one subquad
  // It should be between 2 and 256, and should be a power of 2
  QuadGridMesh(int dimension) : mesh_(dimension/2) {
    assert(2 <= dimension && dimension <= 256);
  }

  void setupPositions(gl::VertexAttribArray attrib) {
    mesh_.setupPositions(attrib);
  }

  // Uses vertex attrib divisor!
  void setupRenderData(gl::VertexAttribArray attrib) {
    mesh_.setupRenderData(attrib);
  }

  // Adds a subquad to the render list.
  // tl = top left, br = bottom right
  void addToRenderList(float offset_x, float offset_y, float scale, float level,
                       bool tl, bool tr, bool bl, bool br) {
    glm::vec4 render_data(offset_x, offset_y, scale, level);
    float dim4 = scale * mesh_.dimension()/2; // our dimension / 4
    if(tl) { mesh_.addToRenderList(render_data + glm::vec4(-dim4, dim4, 0, 0)); }
    if(tr) { mesh_.addToRenderList(render_data + glm::vec4(dim4, dim4, 0, 0)); }
    if(bl) { mesh_.addToRenderList(render_data + glm::vec4(-dim4, -dim4, 0, 0)); }
    if(br) { mesh_.addToRenderList(render_data + glm::vec4(dim4, -dim4, 0, 0)); }
  }

  // Adds all four subquads
  void addToRenderList(float offset_x, float offset_y, float scale, float level) {
    addToRenderList(offset_x, offset_y, scale, level, true, true, true, true);
  }

  void clearRenderList() {
    mesh_.clearRenderList();
  }

  // render with vertex attrib divisor
  void render() const {
    mesh_.render();
  }

  // render with uniforms
  void render(gl::UniformObject<glm::vec4> uRenderData) const {
    mesh_.render(uRenderData);
  }
};

} // namespace cdlod

} // namespace engine

#endif
