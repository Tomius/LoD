// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_TERRAIN_MESH_H_
#define ENGINE_CDLOD_TERRAIN_MESH_H_

#include "../oglwrap_config.h"

#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/textures/texture_2D.h"

#include "./quad_tree.h"
#include "../shader_manager.h"

namespace engine {

namespace cdlod {

class TerrainMesh {
 public:
  explicit TerrainMesh(engine::ShaderManager* manager,
                       const HeightMapInterface& height_map);
  void setup(const gl::Program& program, int tex_unit);
  void render(const Camera& cam);
  const HeightMapInterface& height_map() { return height_map_; }

 private:
  QuadTree mesh_;
  gl::Texture2D height_map_tex_;
  std::unique_ptr<gl::LazyUniform<glm::vec4>> uRenderData_;
  std::unique_ptr<gl::LazyUniform<glm::vec3>> uCamPos_;
  const HeightMapInterface& height_map_;
  int tex_unit_;
};

}  // namespace cdlod

}  // namespace engine

#endif
