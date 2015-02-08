// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SHADOW_H_
#define LOD_SHADOW_H_

#include <vector>
#include "engine/oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/framebuffer.h"
#include "engine/game_object.h"

class Skybox;

class Shadow : public engine::GameObject {
 public:
  Shadow(GameObject* parent, Skybox* skybox, int shadow_map_size,
         int atlas_x_size, int atlas_y_size);
  virtual void screenResized(size_t width, size_t height) override;
  glm::mat4 projMat(float size) const;
  glm::mat4 camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const;
  glm::mat4 modelCamProjMat(glm::vec4 targetBSphere,
                            glm::mat4 modelMatrix,
                            glm::mat4 worldTransform = glm::mat4());

  const std::vector<glm::mat4>& shadowCPs() const;
  const gl::Texture2D& shadowTex() const;
  glm::ivec2 getAtlasDimensions() const {
    return glm::ivec2(xsize_, ysize_);
  }

  void setViewPort();
  void begin();
  void push();
  size_t getDepth() const;
  size_t getMaxDepth() const;
  void set_default_fbo(gl::Framebuffer *default_fbo) {
    default_fbo_ = default_fbo;
  }
  void end();

 private:
  gl::Texture2D tex_;
  gl::Framebuffer fbo_, *default_fbo_;

  size_t w_, h_, size_;
  size_t xsize_, ysize_, curr_depth_, max_depth_;
  std::vector<glm::mat4> cp_matrices_;

  Skybox* skybox_;
};

#endif  // LOD_SHADOW_H_
