#ifndef LOD_TERRAIN_HPP_
#define LOD_TERRAIN_HPP_

#include "terrainMesh.hpp"
#include "skybox.hpp"

class Terrain {
  oglwrap::Program prog_;
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uShadowCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<glm::vec3> uScales_;
  oglwrap::LazyUniform<glm::ivec2> uOffset_;
  oglwrap::LazyUniform<int> uMipmapLevel_;
  TerrainMesh mesh_;

  Skybox& skybox_;
  int w_, h_;
public:
  const int& w, h;

  Terrain(Skybox& skybox);
  void resize(const glm::mat4& projMat);
  glm::vec3 getScales() const;
  void render(float time, const glm::mat4& camMat, const glm::vec3& camPos,
              const glm::mat4& shadowCP, const oglwrap::Texture2D_Array& shadowTex);
  unsigned char fetchHeight(glm::ivec2 v) const;
  double getHeight(double x, double y) const;
};

#endif // LOD_TERRAIN_HPP_
