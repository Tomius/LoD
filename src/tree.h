// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TREE_H_
#define LOD_TREE_H_

#include <vector>
#include <array>

#include "engine/oglwrap_config.h"
#include "engine/scene.h"
#include "engine/game_object.h"
#include "engine/shader_manager.h"
#include "engine/mesh/mesh_renderer.h"
#include "engine/height_map_interface.h"

class Tree : public engine::GameObject {
 public:
  Tree(GameObject *parent, const engine::HeightMapInterface& height_map);
  virtual ~Tree() {}
  virtual void shadowRender() override;
  virtual void render() override;

 private:
  // It should be std::array<engine::MeshRenderer, 3>, but calling its ctor
  // in the initializer list causes sigsegv in the visual c++ compiler.
  std::array<std::unique_ptr<engine::MeshRenderer>, 3> meshes_;
  engine::ShaderProgram prog_, shadow_prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uModelCameraMatrix_;
  gl::LazyUniform<glm::mat3> uNormalMatrix_;
  gl::LazyUniform<glm::mat4> shadow_uMCP_;

  struct TreeInfo {
    int type;
    glm::mat4 mat;
    glm::vec4 bsphere;
    engine::BoundingBox bbox;
  };

  std::vector<TreeInfo> trees_;
};

#endif  // LOD_TREE_H_
