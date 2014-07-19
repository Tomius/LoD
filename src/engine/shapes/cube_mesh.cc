#include "cube_mesh.h"

namespace engine {
namespace shapes {

gl::Cube *CubeMesh::cube_{nullptr};
engine::ShaderProgram *CubeMesh::prog_{nullptr};
gl::LazyUniform<glm::mat4> *CubeMesh::uProjectionMatrix_,
                           *CubeMesh::uCameraMatrix_, *CubeMesh::uModelMatrix_;
gl::LazyUniform<glm::vec3> *CubeMesh::uColor_;

}
}

