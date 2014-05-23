// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TREE_H_
#define LOD_TREE_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"

#include "engine/gameobject.h"
#include "engine/mesh/mesh_renderer.h"

#include "charmove.h"
#include "skybox.h"
#include "shadow.h"
#include "engine/height_map_interface.h"

/* 0 -> max quality
   2 -> max performance */
extern const int PERFORMANCE;

class Tree : public engine::GameObject {
  static constexpr int kTreeTypeNum = 2;
  engine::MeshRenderer mesh_[kTreeTypeNum];
  oglwrap::Program prog_/*, shadow_prog_*/;

  oglwrap::VertexShader vs_/*, shadow_vs_*/;
  oglwrap::FragmentShader fs_/*, shadow_fs_*/;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uModelCameraMatrix_;
  oglwrap::LazyUniform<glm::mat3> uNormalMatrix_;
  //oglwrap::LazyUniform<glm::mat4> shadow_uMCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  Skybox *skybox_;
  //Shadow *shadow_;

  struct TreeInfo {
    int type;
    glm::vec3 pos;
    glm::mat4 mat;
  };

  std::vector<TreeInfo> trees_;

public:
  Tree(const engine::HeightMapInterface& height_map, Skybox *skybox/*, Shadow *shadow*/);
  virtual ~Tree() {}
  //virtual void shadowRender(float time, const engine::Camera& cam) override;
  virtual void render(float time, const engine::Camera& cam) override;
};

#endif // LOD_TREE_H_
