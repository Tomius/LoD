// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TERRAIN_H_
#define LOD_TERRAIN_H_

#include "skybox.h"
#include "shadow.h"
#include "./lod_oglwrap_config.h"

#include "engine/gameobject.h"
#include "engine/height_map.h"
#include "engine/cdlod/terrain_mesh.h"

class Terrain : public engine::GameObject {
  gl::Program prog_;
  gl::VertexShader vs_;
  gl::FragmentShader fs_;

  gl::Texture2D grassMaps_[2], grassNormalMap_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uShadowCP_;
  gl::LazyUniform<glm::vec4> uSunData_;
  gl::LazyUniform<int> uNumUsedShadowMaps_;

  engine::HeightMap<unsigned char> height_map_;
  engine::cdlod::TerrainMesh mesh_;

  Skybox *skybox_;
  Shadow *shadow_;
public:

  Terrain(Skybox *skybox, Shadow *shadow);
  virtual ~Terrain() {}

  const engine::HeightMapInterface& height_map() { return height_map_; }
  virtual void render(float time, const engine::Camera& cam) override;
};

#endif // LOD_TERRAIN_H_
