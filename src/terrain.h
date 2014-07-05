// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TERRAIN_H_
#define LOD_TERRAIN_H_

#include "./skybox.h"
#include "./shadow.h"
#include "engine/oglwrap_config.h"

#include "engine/height_map.h"
#include "engine/game_object.h"
#include "engine/shader_manager.h"
#include "engine/cdlod/terrain_mesh.h"

class Terrain : public engine::GameObject {
 public:
  explicit Terrain(engine::GameObject* parent);
  virtual ~Terrain() {}

  const engine::HeightMapInterface& height_map() { return height_map_; }

 private:
  engine::HeightMap<GLushort> height_map_;
  engine::cdlod::TerrainMesh mesh_;
  engine::ShaderProgram prog_;  // has to be inited after mesh_

  gl::Texture2D grassMaps_[2], grassNormalMap_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_,
                             uModelMatrix_, uShadowCP_;
  gl::LazyUniform<int> uNumUsedShadowMaps_;
  gl::LazyUniform<glm::ivec2> uShadowAtlasSize_;

  virtual void render() override;
  const engine::Transform& initTransform();
};

#endif  // LOD_TERRAIN_H_
