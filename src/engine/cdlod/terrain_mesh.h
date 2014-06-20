// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_TERRAIN_MESH_H_
#define ENGINE_CDLOD_TERRAIN_MESH_H_

#include "../oglwrap_config.h"

#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/textures/texture_2D.h"

#include "./quad_tree.h"

namespace engine {

namespace cdlod {

class TerrainMesh {
 public:
  TerrainMesh(const HeightMapInterface& height_map);

  // It defines the following functions:
  // vec3 CDLODTerrain_worldPos();
  // vec2 CDLODTerrain_texCoord(vec3 pos);
  // vec3 CDLODTerrain_normal(vec3 pos);
  // mat3 CDLODTerrain_normalMatrix(vec3 normal);
  const gl::VertexShader& vertex_shader() const { return vertex_shader_; }

  void setup(gl::Program& program, int tex_unit);

  void setup_and_link(gl::Program& program, int tex_unit) {
    program << vertex_shader_;
    program.link();

    setup(program, tex_unit);
  }

  void render(const Camera& cam);

  const HeightMapInterface& height_map() {
    return height_map_;
  }

 private:
  QuadTree mesh_;
  gl::Texture2D height_map_tex_;
  std::unique_ptr<gl::LazyUniform<glm::vec4>> uRenderData_;
  std::unique_ptr<gl::LazyUniform<glm::vec3>> uCamPos_;
  gl::VertexShader vertex_shader_;
  const HeightMapInterface& height_map_;
  int tex_unit_;
};

} // namespace cdlod

} // namespace engine

#endif
