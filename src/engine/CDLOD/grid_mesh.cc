#include "grid_mesh.h"
#include <limits>
#include <iostream>
#include "../../oglwrap/context.h"

namespace engine {

static oglwrap::Context gl;

GridMesh::GridMesh() : dimension_(0) { }

GLushort GridMesh::indexOf(int sub_quad, int x, int y) {
  GLubyte dim2 = dimension_/2;
  int sub_quad_vertex_cnt = (dim2+1)*(dim2+1);
  int sub_quad_offs = sub_quad * sub_quad_vertex_cnt;
  return sub_quad_offs + (dim2+1)*y + x;
}

void GridMesh::setupPositions(oglwrap::VertexAttribArray attrib, GLubyte dim) {
  dimension_ = dim;
  std::vector<svec2> positions;
  positions.reserve(pow(dim+1, 2));

  GLubyte dim2 = dim/2;

  for (int ysign = 1; ysign >= -1; ysign -= 2) {
    for (int xsign = -1; xsign <= 1; xsign += 2) {
      for (int y = 0; y <= dim2; ++y) {
        for (int x = 0; x <= dim2; ++x) {
          positions.push_back(svec2(xsign*x, ysign*y));
        }
      }
    }
  }

  std::vector<GLushort> indices;
  int sub_quad_index_cnt = 6*dim2*dim2;
  indices.reserve(4 * sub_quad_index_cnt);

  int sub_quad = 0;
  for (int ysign = 1; ysign >= -1; ysign -= 2) {
    for (int xsign = -1; xsign <= 1; xsign += 2, ++sub_quad) {
      subquad_index_start_[sub_quad] =
        static_cast<GLushort*>(0) + indices.size();

      for (int y = 0; y < dim2; ++y) {
        for (int x = 0; x < dim2; ++x) {
          // We should keep CCW winding order
          if(xsign * ysign > 0) {
            indices.push_back( indexOf(sub_quad, x, y) );
            indices.push_back( indexOf(sub_quad, x, y+1) );
            indices.push_back( indexOf(sub_quad, x+1, y) );

            indices.push_back( indexOf(sub_quad, x+1, y) );
            indices.push_back( indexOf(sub_quad, x, y+1) );
            indices.push_back( indexOf(sub_quad, x+1, y+1) );
          } else {
            indices.push_back( indexOf(sub_quad, x, y) );
            indices.push_back( indexOf(sub_quad, x+1, y+1) );
            indices.push_back( indexOf(sub_quad, x, y+1) );

            indices.push_back( indexOf(sub_quad, x, y) );
            indices.push_back( indexOf(sub_quad, x+1, y) );
            indices.push_back( indexOf(sub_quad, x+1, y+1) );
          }
        }
      }

    }
  }

  vao_.bind();
  aPositions_.bind();
  aPositions_.data(positions);
  attrib.pointer(2, oglwrap::DataType::Short).enable();

  aIndices_.bind();
  aIndices_.data(indices);
  vao_.unbind();
}

void GridMesh::setupRenderData(oglwrap::VertexAttribArray attrib) {
  vao_.bind();
  aRenderData_.bind();
  attrib.setup<glm::vec4>().enable();
  attrib.divisor(1);
  vao_.unbind();
}

void GridMesh::drawSubquad(int quad_num, int start_quad_idx) const {
  using oglwrap::PrimType;
  const int sub_quad_size = 3 * dimension_ * dimension_ / 2;

  gl.DrawElements(PrimType::Triangles,
                  quad_num*sub_quad_size,
                  subquad_index_start_[start_quad_idx]);
}

// This is the core of the CDLOD terrain renderer, so it should be fast.
// Notice, that the all subsets of the 4 sub-quads can be rendered with
// glDrawElements if we use index offsets.
void GridMesh::renderImmediately(bool tl, bool tr, bool bl, bool br) {
  bool _0 = tl, _1 = tr, _2 = bl, _3 = br;

  vao_.bind();
  if (_3) {
    if (_2) {
      if (_1) {
        if (_0) { // 0, 1, 2, 3
          drawSubquad(4, 0);
        } else { // 1, 2, 3
          drawSubquad(3, 1);
        }
      } else {
        if (_0) { // 0, 2, 3
          drawSubquad(1, 0);
          drawSubquad(2, 2);
        } else { // 2, 3
          drawSubquad(2, 2);
        }
      }
    } else {
      if (_1) {
        if (_0) { // 0, 1, 3
          drawSubquad(2, 0);
          drawSubquad(1, 3);
        } else { // 1, 3
          drawSubquad(1, 1);
          drawSubquad(1, 3);
        }
      } else {
        if (_0) { // 0, 3
          drawSubquad(1, 0);
          drawSubquad(1, 3);
        } else { // 3
          drawSubquad(1, 3);
        }
      }
    }
  } else {
    if (_2) {
      if (_1) {
        if (_0) { // 0, 1, 2
          drawSubquad(3, 0);
        } else { // 1, 2
          drawSubquad(2, 1);
        }
      } else {
        if (_0) { // 0, 2
          drawSubquad(1, 0);
          drawSubquad(1, 2);
        } else { // 2
          drawSubquad(1, 2);
        }
      }
    } else {
      if (_1) {
        if (_0) { // 0, 1
          drawSubquad(2, 0);
        } else { // 1
          drawSubquad(1, 1);
        }
      } else {
        if (_0) { // 0
          drawSubquad(1, 0);
        }
      }
    }
  }

  vao_.unbind();
}

void GridMesh::addToRenderList(const glm::vec4& render_data,
                               bool tl, bool tr, bool bl, bool br) {
  if (tl) { render_data_[0].push_back(render_data); }
  if (tr) { render_data_[1].push_back(render_data); }
  if (bl) { render_data_[2].push_back(render_data); }
  if (br) { render_data_[3].push_back(render_data); }
}

void GridMesh::clearRenderList() {
  for (int i = 0; i < 4; ++i) {
    render_data_[i].erase(render_data_[i].begin(), render_data_[i].end());
  }
}

void GridMesh::render() const {
  using oglwrap::PrimType;
  const int sub_quad_size = 3 * dimension_ * dimension_ / 2;
  vao_.bind();
  aRenderData_.bind();
  for (int i = 0; i < 4; ++i) {
    aRenderData_.data(render_data_[i]);

    gl.DrawElementsInstanced(PrimType::Triangles,       // type
                             sub_quad_size,             // count per instance
                             subquad_index_start_[i],   // idx offset
                             render_data_[i].size());   // instance count

  }
  vao_.unbind();
}

} // namespace engine

