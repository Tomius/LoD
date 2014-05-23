#include "grid_mesh.h"

#include "../../oglwrap/context.h"

namespace engine {

namespace cdlod {

using gl = oglwrap::Context;

GridMesh::GridMesh(GLubyte dimension) : dimension_(dimension) { }

GLushort GridMesh::indexOf(int x, int y) {
  x += dimension_/2;
  y += dimension_/2;
  return (dimension_ + 1) * y + x;
}

void GridMesh::setupPositions(oglwrap::VertexAttribArray attrib) {
  std::vector<svec2> positions;
  positions.reserve((dimension_+1) * (dimension_+1));

  GLubyte dim2 = dimension_/2;

  for (int y = -dim2; y <= dim2; ++y) {
    for (int x = -dim2; x <= dim2; ++x) {
      positions.push_back(svec2(x, y));
    }
  }

  std::vector<GLushort> indices;
  index_count_ = 6*dimension_*dimension_;
  indices.reserve(index_count_);

  for (int y = -dim2; y < dim2; ++y) {
    for (int x = -dim2; x < dim2; ++x) {
      // We should keep CCW winding order
        indices.push_back( indexOf(x, y) );
        indices.push_back( indexOf(x, y+1) );
        indices.push_back( indexOf(x+1, y) );

        indices.push_back( indexOf(x+1, y) );
        indices.push_back( indexOf(x, y+1) );
        indices.push_back( indexOf(x+1, y+1) );
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
#ifdef glVertexAttribDivisor
  if (glVertexAttribDivisor) {
    vao_.bind();
    aRenderData_.bind();
    attrib.setup<glm::vec4>().enable();
    attrib.divisor(1);
    vao_.unbind();
  }
#endif
}

void GridMesh::addToRenderList(const glm::vec4& render_data) {
  render_data_.push_back(render_data);
}

void GridMesh::clearRenderList() {
  render_data_.erase(render_data_.begin(), render_data_.end());
}

void GridMesh::render() const {
#if defined(glDrawElementsInstanced) && defined(glVertexAttribDivisor)
  if (glVertexAttribDivisor) {
    using oglwrap::PrimType;
    using oglwrap::IndexType;

    vao_.bind();

    aRenderData_.bind();
    aRenderData_.data(render_data_);

    gl::DrawElementsInstanced(PrimType::Triangles,
                             index_count_,
                             IndexType::UnsignedShort,
                             render_data_.size());   // instance count
    vao_.unbind();
  }
#endif
}

void GridMesh::render(oglwrap::UniformObject<glm::vec4> uRenderData) const {
  using oglwrap::PrimType;
  using oglwrap::IndexType;

  vao_.bind();
  for(auto& data : render_data_) {
    uRenderData = data;
    gl::DrawElements(PrimType::Triangles,
                    index_count_,
                    IndexType::UnsignedShort);
  }
  vao_.unbind();
}

} // namespace cdlod

} // namespace engine

