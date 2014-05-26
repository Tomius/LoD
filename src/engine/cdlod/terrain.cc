#include "./terrain.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {

namespace cdlod {

Terrain::Terrain(const HeightMapInterface& height_map)
    : mesh_(height_map), height_map_(height_map) {

  oglwrap::ShaderSource vs_src{"cdlod_terrain.vert"};

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor)
      vs_src.insertMacroValue("VERTEX_ATTRIB_DIVISOR", true);
    else
  #endif
      vs_src.insertMacroValue("VERTEX_ATTRIB_DIVISOR", false);

  vertex_shader_.source(vs_src);
}

void Terrain::setup(oglwrap::Program& program, int tex_unit) {
  namespace gl = oglwrap;
  using glEnum = oglwrap::SmartEnums;

  program.use();

  mesh_.setupPositions(program | "CDLODTerrain_aPosition");

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor) {
      mesh_.setupRenderData(program | "CDLODTerrain_uRenderData");
    } else
  #endif
    {
      uRenderData_ = make_unique<gl::LazyUniform<glm::vec4>>(
          program, "CDLODTerrain_uRenderData");
    }

  uCamPos_ = make_unique<gl::LazyUniform<glm::vec3>>(
      program, "CDLODTerrain_uCamPos");

  tex_unit_ = tex_unit;
  gl::UniformSampler(program, "CDLODTerrain_uHeightMap") = tex_unit;
  gl::Uniform<glm::vec2>(program, "CDLODTerrain_uTexSize") =
      glm::vec2(height_map_.w(), height_map_.h());

  height_map_tex_.active(tex_unit);
  height_map_tex_.bind();
  height_map_.upload(height_map_tex_);
  height_map_tex_.minFilter(glEnum::Linear);
  height_map_tex_.magFilter(glEnum::Linear);
  height_map_tex_.unbind();
}

void Terrain::render(const Camera& cam) {
  namespace gl = oglwrap;
  using glEnum = oglwrap::SmartEnums;

  if(!uCamPos_) {
    throw std::logic_error("engine::cdlod::terrain requires a setup() call, "
                           "before the use of the render() function.");
  }

  height_map_tex_.active(tex_unit_);
  height_map_tex_.bind();

  uCamPos_->set(cam.pos());

  gl::FrontFace(glEnum::Ccw);
  gl::TemporaryEnable cullface{glEnum::CullFace};

  #ifdef glVertexAttribDivisor
    if (glVertexAttribDivisor)
      mesh_.render(cam);
    else
  #endif
    mesh_.render(cam, *uRenderData_);

  height_map_tex_.active(tex_unit_);
  height_map_tex_.unbind();
}

} // namespace cdlod

} // namespace engine
