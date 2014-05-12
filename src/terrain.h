// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TERRAIN_H_
#define LOD_TERRAIN_H_

#include "terrain_mesh.h"
#include "skybox.h"
#include "shadow.h"
#include "./lod_oglwrap_config.h"

#include "engine/gameobject.h"

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

  Skybox *skybox_;
  Shadow *shadow_;
  int w_, h_;
public:
  const int& w, h;

  Terrain(Skybox *skybox, Shadow *shadow);
  virtual ~Terrain() {}

  glm::vec3 getScales() const;
  unsigned char fetchHeight(glm::ivec2 v) const;
  double getHeight(double x, double y) const;

  virtual void screenResized(const glm::mat4& projMat, size_t, size_t) override;
  virtual void render(float time, const engine::Camera& cam) override;
};

#endif // LOD_TERRAIN_H_
