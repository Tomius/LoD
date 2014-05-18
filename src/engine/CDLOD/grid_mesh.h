// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_H_
#define ENGINE_CDLOD_GRID_MESH_H_

#include "../../lod_oglwrap_config.h"
#include "../../oglwrap/buffer.h"
#include "../../oglwrap/vertex_attrib.h"

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
// use unsigned shorts instead of ints or floats)
class GridMesh {
  oglwrap::VertexArray vao_;
  oglwrap::IndexBuffer indices_;
  oglwrap::ArrayBuffer positions_;
  GLubyte dimension_;
  GLushort* subquad_index_start_[4];

  GLushort indexOf(int sub_quad, int x, int y);
  void drawSubquad(int quad_num, int start_quad_idx) const;

 public:
  GridMesh();
  void setupPositions(oglwrap::VertexAttribArray attrib, GLubyte dim);
  // The arguments specify which quarter parts are to be rendered.
  // TL = top left, BR = bottom right
  void render(bool TL = true, bool TR = true, bool BL = true, bool BR = true);
};

#endif
