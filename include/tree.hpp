#ifndef LOD_TREE_HPP_
#define LOD_TREE_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/shader.hpp"
#include "oglwrap/uniform.hpp"

#include "../engine/gameobject.hpp"
#include "../engine/mesh/mesh_renderer.hpp"

#include "charmove.hpp"
#include "skybox.hpp"
#include "terrain.hpp"
#include "shadow.hpp"

/* 0 -> max quality
   2 -> max performance */
extern const int PERFORMANCE;

class Tree : public engine::GameObject {
  engine::MeshRenderer mesh_;
  oglwrap::Program prog_, shadow_prog_;

  oglwrap::VertexShader vs_, shadow_vs_;
  oglwrap::FragmentShader fs_, shadow_fs_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uModelCameraMatrix_;
  oglwrap::LazyUniform<glm::mat3> uNormalMatrix_;
  oglwrap::LazyUniform<glm::mat4> shadow_uMCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  glm::vec3 scales_;
  Skybox& skybox_;
  Shadow& shadow_;

  struct TreeInfo {
    glm::vec3 pos;
    glm::mat4 mat;
    TreeInfo(const glm::vec3& pos, const glm::mat4& mat) : pos(pos), mat(mat) { }
  };

  std::vector<TreeInfo> trees_;

public:
  Tree(Skybox& skybox, Shadow& shadow, const Terrain& terrain);
  void screenResized(const glm::mat4& projMat, size_t, size_t) override;
  void shadowRender(float time, const engine::Camera& cam) override;
  void render(float time, const engine::Camera& cam) override;
};

#endif // LOD_TREE_HPP_
