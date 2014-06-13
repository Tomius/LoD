// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TERRAIN_H_
#define LOD_TERRAIN_H_

#include "skybox.h"
#include "shadow.h"
#include "./lod_oglwrap_config.h"

#include "engine/game_object.h"
#include "engine/height_map.h"
#include "engine/cdlod/terrain_mesh.h"

class Terrain : public engine::GameObject {
  gl::Program prog_;
  gl::VertexShader vs_;
  gl::FragmentShader fs_;

  gl::Texture2D grassMaps_[2], grassNormalMap_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_,
                             uModelMatrix_, uShadowCP_;
  gl::LazyUniform<glm::vec3> uSunPos_;
  gl::LazyUniform<int> uNumUsedShadowMaps_;
  gl::LazyUniform<glm::ivec2> uShadowAtlasSize_;

  engine::HeightMap<unsigned short> height_map_;
  engine::cdlod::TerrainMesh mesh_;

  Skybox *skybox_;

  const engine::Transform& initTransform();

public:
  Terrain(Skybox *skybox);
  virtual ~Terrain() {}

  const engine::HeightMapInterface& height_map() { return height_map_; }
  virtual void render(const engine::Scene& scene) override;
};

#endif  // LOD_TERRAIN_H_
