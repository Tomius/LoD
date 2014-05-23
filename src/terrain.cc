// Copyright (c) 2014, Tamas Csala

#include "terrain.h"
#include <string>

using namespace oglwrap;

Terrain::Terrain(Skybox *skybox/*, Shadow *shadow*/)
  : vs_("terrain.vert")
  , fs_("terrain.frag")
  , uProjectionMatrix_(prog_, "uProjectionMatrix")
  , uCameraMatrix_(prog_, "uCameraMatrix")
  //, uShadowCP_(prog_, "uShadowCP")
  , uSunData_(prog_, "uSunData")
  //, uNumUsedShadowMaps_(prog_, "uNumUsedShadowMaps")
  , height_map_("terrain/konserian.png")
  , mesh_(height_map_)
  , skybox_((assert(skybox), skybox))
  /*, shadow_((assert(shadow), shadow))*/ {

  prog_ << vs_ << fs_ << skybox_->sky_fs;
  mesh_.setup_and_link(prog_, 1);
  prog_.use();

  UniformSampler(prog_, "uEnvMap").set(0);
  UniformSampler(prog_, "uGrassMap0").set(2);
  UniformSampler(prog_, "uGrassMap1").set(3);
  for (int i = 0; i < 2; ++i) {
    grassMaps_[i].bind();
    // no alpha channel here
    grassMaps_[i].loadTexture(
      i == 0 ? "textures/grass.jpg" : "textures/grass_2.jpg", "SRGBA"
    );
    grassMaps_[i].generateMipmap();
    grassMaps_[i].maxAnisotropy();
    grassMaps_[i].minFilter(MinFilter::LinearMipmapLinear);
    grassMaps_[i].magFilter(MagFilter::Linear);
    grassMaps_[i].wrapS(WrapMode::Repeat);
    grassMaps_[i].wrapT(WrapMode::Repeat);
  }

  UniformSampler(prog_, "uGrassNormalMap").set(4);
  grassNormalMap_.bind();
  {
    // the normal map doesn't have an alpha channel, and is not is srgb space
    grassNormalMap_.loadTexture("textures/grass_normal.jpg", "RGBA");
    grassNormalMap_.generateMipmap();
    grassNormalMap_.minFilter(MinFilter::LinearMipmapLinear);
    grassNormalMap_.magFilter(MagFilter::Linear);
    grassNormalMap_.wrapS(WrapMode::Repeat);
    grassNormalMap_.wrapT(WrapMode::Repeat);
  }

  //UniformSampler(prog_, "uShadowMap").set(5);
  //Uniform<glm::ivec2>(prog_, "uShadowAtlasSize") = shadow->getAtlasDimensions();

  prog_.validate();
}

void Terrain::render(float time, const engine::Camera& cam) {
  prog_.use();
  uCameraMatrix_ = cam.matrix();
  uProjectionMatrix_ = cam.projectionMatrix();
  uSunData_.set(skybox_->getSunData());
  // for (size_t i = 0; i < shadow_->getDepth(); ++i) {
  //   uShadowCP_[i] = shadow_->shadowCPs()[i];
  // }
  // uNumUsedShadowMaps_ = shadow_->getDepth();
  skybox_->env_map.active(0);
  skybox_->env_map.bind();
  grassMaps_[0].active(2);
  grassMaps_[0].bind();
  grassMaps_[1].active(3);
  grassMaps_[1].bind();
  grassNormalMap_.active(4);
  grassNormalMap_.bind();
  // shadow_->shadowTex().active(5);
  // shadow_->shadowTex().bind();

  mesh_.render(cam);

  // shadow_->shadowTex().active(5);
  // shadow_->shadowTex().unbind();
  grassMaps_[0].active(2);
  grassMaps_[0].unbind();
  grassMaps_[1].active(3);
  grassMaps_[1].unbind();
  grassNormalMap_.active(4);
  grassNormalMap_.unbind();
  skybox_->env_map.active(0);
  skybox_->env_map.unbind();
}


