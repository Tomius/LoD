#include "grid_mesh.h"

#include "../../oglwrap/context.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {
namespace cdlod {

GridMesh::GridMesh(GLubyte dimension) : dimension_(dimension) { }

GLushort GridMesh::indexOf(int x, int y) {
  x += dimension_/2;
  y += dimension_/2;
  return (dimension_ + 1) * y + x;
}

void GridMesh::setupPositions(gl::VertexAttrib attrib) {
  std::vector<svec2> positions;
  positions.reserve((dimension_+1) * (dimension_+1));

  GLubyte dim2 = dimension_/2;

  for (int y = -dim2; y <= dim2; ++y) {
    for (int x = -dim2; x <= dim2; ++x) {
      positions.push_back(svec2(x, y));
    }
  }

  std::vector<GLushort> indices;
  index_count_ = 2*(dimension_+1)*(dimension_) + 2*dimension_;
  indices.reserve(index_count_);

  for (int y = -dim2; y < dim2; ++y) {
    for (int x = -dim2; x <= dim2; ++x) {
      indices.push_back( indexOf(x, y) );
      indices.push_back( indexOf(x, y+1) );
    }
    // create a degenerate (as primitive restart)
    indices.push_back(indexOf(dim2, y+1));
    indices.push_back(indexOf(-dim2, y+1));
  }

  gl::Bind(vao_);
  gl::Bind(aPositions_);
  aPositions_.data(positions);
  attrib.pointer(2, gl::DataType::kShort).enable();
  gl::Unbind(aPositions_);

  gl::Bind(aIndices_);
  aIndices_.data(indices);
  gl::Unbind(vao_);
}

void GridMesh::setupRenderData(gl::VertexAttrib attrib) {
#ifdef glVertexAttribDivisor
  if (glVertexAttribDivisor) {
    gl::Bind(vao_);
    gl::Bind(aRenderData_);
    attrib.setup<glm::vec4>().enable();
    attrib.divisor(1);
    gl::Unbind(vao_);
  }
#endif
}

void GridMesh::addToRenderList(const glm::vec4& render_data) {
  render_data_.push_back(render_data);
}

void GridMesh::clearRenderList() {
  render_data_.erase(render_data_.begin(), render_data_.end());
}

void GridMesh::render() {
#if defined(glDrawElementsInstanced) && defined(glVertexAttribDivisor)
  if (glVertexAttribDivisor) {
    using gl::PrimType;
    using gl::IndexType;

    gl::Bind(vao_);
    gl::Bind(aRenderData_);
    aRenderData_.data(render_data_);

    gl::DrawElementsInstanced(PrimType::kTriangleStrip,
                              index_count_,
                              IndexType::kUnsignedShort,
                              render_data_.size());   // instance count
    gl::Unbind(vao_);
  }
#endif
}

void GridMesh::render(gl::UniformObject<glm::vec4> uRenderData) const {
  using gl::PrimType;
  using gl::IndexType;

  gl::Bind(vao_);
  for(auto& data : render_data_) {
    uRenderData = data;
    gl::DrawElements(PrimType::kTriangleStrip,
                    index_count_,
                    IndexType::kUnsignedShort);
  }
  gl::Unbind(vao_);
}

} // namespace cdlod

} // namespace engine

