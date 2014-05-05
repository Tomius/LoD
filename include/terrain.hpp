#ifndef LOD_TERRAIN_HPP_
#define LOD_TERRAIN_HPP_

#include "terrainMesh.hpp"
#include "skybox.hpp"
#include "shadow.hpp"
#include "oglwrap_config.hpp"
#include "oglwrap/utils/camera.hpp"
#include "../engine/gameobject.hpp"

class Terrain : public engine::GameObject {
  oglwrap::Program prog_;
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uShadowCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<glm::vec3> uScales_;
  oglwrap::LazyUniform<glm::ivec2> uOffset_, uTexSize_;
  oglwrap::LazyUniform<int> uMipmapLevel_, uNumUsedShadowMaps_;
  TerrainMesh mesh_;

  Skybox& skybox_;
  Shadow& shadow_;
  int w_, h_;
public:
  const int& w, h;

  Terrain(Skybox& skybox, Shadow& shadow);
  glm::vec3 getScales() const;
  unsigned char fetchHeight(glm::ivec2 v) const;
  double getHeight(double x, double y) const;

  void screenResized(const glm::mat4& projMat, GLuint, GLuint) override;
  void render(float time, const oglwrap::Camera& cam) override;
};

#endif // LOD_TERRAIN_HPP_
