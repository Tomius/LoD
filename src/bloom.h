// Copyright (c) 2014, Tamas Csala

#ifndef LOD_BLOOM_H_
#define LOD_BLOOM_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/fullScreenRect.h"
#include "oglwrap/textures/texture_2D.h"

#include "engine/gameobject.h"

class BloomEffect : public engine::GameObject {
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::Texture2D tex_;
  oglwrap::LazyUniform<glm::vec2> uScreenSize_;
  oglwrap::FullScreenRectangle rect_;

  GLuint width_, height_;
public:
  BloomEffect();
  virtual ~BloomEffect() {}
  virtual void screenResized(const glm::mat4&, size_t width, size_t height) override;
  virtual void render(float, const engine::Camera&) override;
};


#endif // LOD_BLOOM_H_
