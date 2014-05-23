// Copyright (c) 2014, Tamas Csala

#ifndef LOD_TERRAIN_H_
#define LOD_TERRAIN_H_

#include "skybox.h"
#include "shadow.h"
#include "./lod_oglwrap_config.h"

#include "engine/gameobject.h"
#include "engine/height_map.h"
#include "engine/cdlod/terrain.h"

class Terrain : public engine::GameObject {
  oglwrap::Program prog_;
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;

  oglwrap::Texture2D grassMaps_[2], grassNormalMap_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_/*, uShadowCP_*/;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  //oglwrap::LazyUniform<int> uNumUsedShadowMaps_;

  engine::HeightMap<unsigned char> height_map_;
  engine::cdlod::Terrain mesh_;

  Skybox *skybox_;
  //Shadow *shadow_;
public:

  Terrain(Skybox *skybox/*, Shadow *shadow*/);
  virtual ~Terrain() {}

  const engine::HeightMapInterface& height_map() { return height_map_; }
  virtual void render(float time, const engine::Camera& cam) override;
};

#endif // LOD_TERRAIN_H_
