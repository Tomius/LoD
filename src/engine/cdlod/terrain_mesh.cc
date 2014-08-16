// Copyright (c) 2014, Tamas Csala

#include "./terrain_mesh.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {
namespace cdlod {

TerrainMesh::TerrainMesh(engine::ShaderManager* manager,
                         const HeightMapInterface& height_map)
    : mesh_(height_map), height_map_(height_map) {
  gl::ShaderSource vs_src{"engine/cdlod_terrain.vert"};

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor)
      vs_src.insertMacroValue("VERTEX_ATTRIB_DIVISOR", true);
    else
  #endif
      vs_src.insertMacroValue("VERTEX_ATTRIB_DIVISOR", false);

  manager->publish("engine/cdlod_terrain.vert", vs_src);
}

void TerrainMesh::setup(const gl::Program& program, gl::TextureLayout& layout) {
  gl::Use(program);

  mesh_.setupPositions(program | "CDLODTerrain_aPosition");

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor) {
      mesh_.setupRenderData(program | "CDLODTerrain_uRenderData");
    } else
  #endif
    {
      uRenderData_ = engine::make_unique<gl::LazyUniform<glm::vec4>>(
          program, "CDLODTerrain_uRenderData");
    }

  uCamPos_ = engine::make_unique<gl::LazyUniform<glm::vec3>>(
      program, "CDLODTerrain_uCamPos");

  gl::UniformSampler(program, "CDLODTerrain_uHeightMap") =
    layout.add(height_map_tex_);
  gl::Uniform<glm::vec2>(program, "CDLODTerrain_uTexSize") =
      glm::vec2(height_map_.w(), height_map_.h());

  gl::BoundTexture2D tex{height_map_tex_};
  height_map_.upload(tex);
  tex.minFilter(gl::kLinear);
  tex.magFilter(gl::kLinear);
}

void TerrainMesh::render(const Camera& cam) {
  uCamPos_->set(cam.transform()->pos());

  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor)
      mesh_.render(cam);
    else
  #endif
    mesh_.render(cam, *uRenderData_);
}

}  // namespace cdlod
}  // namespace engine
