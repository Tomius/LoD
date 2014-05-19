// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_H_
#define ENGINE_CDLOD_GRID_MESH_H_

#include "../../lod_oglwrap_config.h"
#include "../../oglwrap/buffer.h"
#include "../../oglwrap/vertex_attrib.h"

namespace engine {

// A two-dimensional vector of GLshort values
struct svec2 {
  GLshort x, y;
  svec2() :x(0), y(0) {}
  svec2(GLshort a, GLshort b) : x(a), y(b) {}
  svec2 operator+(const svec2 rhs) { return svec2(x + rhs.x, y + rhs.y); }
};

inline svec2 operator*(GLshort lhs, const svec2 rhs) {
  return svec2(lhs * rhs.x, lhs * rhs.y);
}

// Renders a regular grid mesh, that is of (dimension+1) x (dimension+1) in size
// so a GridMesh(16) will go from (-8, -8) to (8, 8). The gridmesh has 4 subquads
// that can be rendered separately (it is useful for CDLOD)
//
// For performance reasons, GridMesh's maximum size is 255*255 (so that it can
// use unsigned shorts instead of ints or floats), but for CDLOD, you need
// pow2 sizes, so there 128*128 is the max
class GridMesh {
  oglwrap::VertexArray vao_;
  oglwrap::IndexBuffer aIndices_;
  oglwrap::ArrayBuffer aPositions_, aRenderData_;
  GLubyte dimension_;
  GLushort* subquad_index_start_[4];
  std::vector<glm::vec4> render_data_[4]; // xy: offset, z: scale, w: level

  GLushort indexOf(int sub_quad, int x, int y);
  void drawSubquad(int quad_num, int start_quad_idx) const;
  void renderImmediately(bool tl, bool tr, bool bl, bool br);

 public:
  GridMesh();
  void setupPositions(oglwrap::VertexAttribArray attrib, GLubyte dim);
  void setupRenderData(oglwrap::VertexAttribArray attrib);

  // The arguments specify which quarter parts are to be rendered.
  // tl = top left, br = bottom right
  void addToRenderList(const glm::vec4& render_data,
                       bool tl, bool tr, bool bl, bool br);
  void clearRenderList();
  void render() const;
};

} // namespace engine

#endif
