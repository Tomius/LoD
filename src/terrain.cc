// Copyright (c) 2014, Tamas Csala

#include "./terrain.h"
#include <string>

#include "engine/scene.h"

Terrain::Terrain(engine::GameObject* parent)
    : engine::GameObject(parent)
    , height_map_("terrain/output.png")
    , mesh_(scene_->shader_manager(), height_map_)
    , prog_(scene_->shader_manager()->get("terrain.vert"),
            scene_->shader_manager()->get("terrain.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix")
    , uShadowCP_(prog_, "uShadowCP")
    , uNumUsedShadowMaps_(prog_, "uNumUsedShadowMaps")
    , uShadowAtlasSize_(prog_, "uShadowAtlasSize") {
  gl::Use(prog_);
  mesh_.setup(prog_, layout_);
  gl::UniformSampler(prog_, "uGrassMap0").set(layout_.add(grassMaps_[0]));
  gl::UniformSampler(prog_, "uGrassMap1").set(layout_.add(grassMaps_[1]));
  for (int i = 0; i < 2; ++i) {
    gl::BoundTexture2D tex{grassMaps_[i]};
    // no alpha channel here
    tex.loadTexture(
      i == 0 ? "textures/grass.jpg" : "textures/grass_2.jpg", "CSRGB");
    tex.generateMipmap();
    tex.maxAnisotropy();
    tex.minFilter(gl::kLinearMipmapLinear);
    tex.magFilter(gl::kLinear);
    tex.wrapS(gl::kRepeat);
    tex.wrapT(gl::kRepeat);
  }

  gl::UniformSampler(prog_, "uGrassNormalMap").set(layout_.add(grassNormalMap_));
  gl::BoundTexture2D tex{grassNormalMap_};
  // the normal map doesn't have an alpha channel, and is not is srgb space
  tex.loadTexture("textures/grass_normal.jpg", "CRGB");
  tex.generateMipmap();
  tex.minFilter(gl::kLinearMipmapLinear);
  tex.magFilter(gl::kLinear);
  tex.wrapS(gl::kRepeat);
  tex.wrapT(gl::kRepeat);

  Shadow *shadow = scene_->shadow();
  if (shadow) {
    gl::UniformSampler(prog_, "uShadowMap").set(layout_.add(shadow->shadowTex()));
  }

  prog_.validate();
}

void Terrain::render() {
  const engine::Camera& cam = *scene_->camera();
  const Shadow *shadow = scene_->shadow();

  gl::Use(prog_);
  prog_.update();
  uCameraMatrix_ = cam.cameraMatrix();
  uProjectionMatrix_ = cam.projectionMatrix();
  uModelMatrix_ = transform()->matrix();
  if (shadow) {
    for (size_t i = 0; i < shadow->getDepth(); ++i) {
      uShadowCP_[i] = shadow->shadowCPs()[i];
    }
    uNumUsedShadowMaps_ = shadow->getDepth();
    uShadowAtlasSize_ = shadow->getAtlasDimensions();
  }

  layout_.bind();

  mesh_.render(cam);

  layout_.unbind();
}


