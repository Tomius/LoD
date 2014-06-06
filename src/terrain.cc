// Copyright (c) 2014, Tamas Csala

#include "terrain.h"
#include <string>

#include "engine/scene.h"

Terrain::Terrain(Skybox *skybox)
    : vs_("terrain.vert")
    , fs_("terrain.frag")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uShadowCP_(prog_, "uShadowCP")
    , uSunPos_(prog_, "uSunPos")
    , uNumUsedShadowMaps_(prog_, "uNumUsedShadowMaps")
    , uShadowAtlasSize_(prog_, "uShadowAtlasSize")
    , height_map_("terrain/mideu.png")
    , mesh_(height_map_)
    , skybox_((assert(skybox), skybox)) {
  prog_ << vs_ << fs_ << skybox_->sky_fs();
  mesh_.setup_and_link(prog_, 1);
  prog_.use();

  gl::UniformSampler(prog_, "uGrassMap0").set(2);
  gl::UniformSampler(prog_, "uGrassMap1").set(3);
  for (int i = 0; i < 2; ++i) {
    grassMaps_[i].bind();
    // no alpha channel here
    grassMaps_[i].loadTexture(
      i == 0 ? "textures/grass.jpg" : "textures/grass_2.jpg", "CSRGBA"
    );
    grassMaps_[i].generateMipmap();
    grassMaps_[i].maxAnisotropy();
    grassMaps_[i].minFilter(gl::kLinearMipmapLinear);
    grassMaps_[i].magFilter(gl::kLinear);
    grassMaps_[i].wrapS(gl::kRepeat);
    grassMaps_[i].wrapT(gl::kRepeat);
  }

  gl::UniformSampler(prog_, "uGrassNormalMap").set(4);
  grassNormalMap_.bind();
  {
    // the normal map doesn't have an alpha channel, and is not is srgb space
    grassNormalMap_.loadTexture("textures/grass_normal.jpg", "CRGBA");
    grassNormalMap_.generateMipmap();
    grassNormalMap_.minFilter(gl::kLinearMipmapLinear);
    grassNormalMap_.magFilter(gl::kLinear);
    grassNormalMap_.wrapS(gl::kRepeat);
    grassNormalMap_.wrapT(gl::kRepeat);
  }

  gl::UniformSampler(prog_, "uShadowMap").set(5);

  prog_.validate();
}

void Terrain::render(const engine::Scene& scene) {
  const engine::Camera& cam = *scene.camera();
  const Shadow *shadow = scene.shadow();

  prog_.use();
  uCameraMatrix_ = cam.matrix();
  uProjectionMatrix_ = cam.projectionMatrix();
  uSunPos_.set(skybox_->getSunPos());
  if(shadow) {
    for (size_t i = 0; i < shadow->getDepth(); ++i) {
      uShadowCP_[i] = shadow->shadowCPs()[i];
    }
    uNumUsedShadowMaps_ = shadow->getDepth();
    uShadowAtlasSize_ = shadow->getAtlasDimensions();
  }

  grassMaps_[0].bind(2);
  grassMaps_[1].bind(3);
  grassNormalMap_.bind(4);
  if(shadow) { shadow->shadowTex().bind(5); }

  mesh_.render(cam);

  if(shadow) { shadow->shadowTex().unbind(5); }
  grassNormalMap_.unbind(4);
  grassMaps_[1].unbind(3);
  grassMaps_[0].unbind(2);
}


