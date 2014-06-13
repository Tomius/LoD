// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TREE_H_
#define LOD_TREE_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"

#include "engine/game_object.h"
#include "engine/mesh/mesh_renderer.h"

#include "charmove.h"
#include "skybox.h"
#include "shadow.h"
#include "engine/height_map_interface.h"

class Tree : public engine::GameObject {
  static constexpr int kTreeTypeNum = 2;
  engine::MeshRenderer mesh_[kTreeTypeNum];
  gl::Program prog_, shadow_prog_;

  gl::VertexShader vs_, shadow_vs_;
  gl::FragmentShader fs_, shadow_fs_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uModelCameraMatrix_;
  gl::LazyUniform<glm::mat3> uNormalMatrix_;
  gl::LazyUniform<glm::mat4> shadow_uMCP_;
  gl::LazyUniform<glm::vec3> uSunPos_;

  Skybox *skybox_;
  Shadow *shadow_;

  struct TreeInfo {
    int type;
    glm::mat4 mat;
    glm::vec4 bsphere;
    engine::BoundingBox bbox;
  };

  std::vector<TreeInfo> trees_;

public:
  Tree(const engine::HeightMapInterface& height_map, Skybox *skybox, Shadow *shadow);
  virtual ~Tree() {}
  virtual void shadowRender(const engine::Scene& scene) override;
  virtual void render(const engine::Scene& scene) override;
};

#endif  // LOD_TREE_H_
