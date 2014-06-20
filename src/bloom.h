// Copyright (c) 2014, Tamas Csala

#ifndef LOD_BLOOM_H_
#define LOD_BLOOM_H_

#include "engine/oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/full_screen_rect.h"
#include "oglwrap/textures/texture_2D.h"

#include "engine/game_object.h"

class BloomEffect : public engine::GameObject {
  gl::VertexShader vs_;
  gl::FragmentShader fs_;
  gl::Program prog_;

  gl::Texture2D tex_;
  gl::LazyUniform<glm::vec2> uScreenSize_;
  gl::FullScreenRectangle rect_;

  GLuint width_, height_;
 public:
  BloomEffect();
  virtual ~BloomEffect() {}
  virtual void screenResized(const engine::Scene&, size_t width,
                             size_t height) override;
  virtual void render(const engine::Scene& scene) override;
};


#endif  // LOD_BLOOM_H_
