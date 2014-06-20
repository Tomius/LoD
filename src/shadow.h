// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SHADOW_H_
#define LOD_SHADOW_H_

#include "engine/oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/framebuffer.h"
#include "engine/game_object.h"

class Shadow {
  gl::Texture2D tex_;
  gl::Framebuffer fbo_;

  size_t w_, h_, size_;
  size_t xsize_, ysize_, curr_depth_, max_depth_;
  std::vector<glm::mat4> cp_matrices_;

public:
  Shadow(int shadow_map_size, int atlas_x_size, int atlas_y_size);
  void screenResized(size_t width, size_t height);
  glm::dmat4 projMat(double size) const;
  glm::dmat4 camMat(glm::dvec3 lightSrcPos, glm::dvec4 targetBSphere) const;
  glm::mat4 modelCamProjMat(glm::dvec3 lightSrcPos, glm::dvec4 targetBSphere,
                          glm::dmat4 modelMatrix, glm::dmat4 worldTransform = glm::dmat4());
  glm::mat4 modelCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere,
                          glm::mat4 modelMatrix, glm::mat4 worldTransform = glm::mat4()) {
    return modelCamProjMat(glm::dvec3(lightSrcPos), glm::dvec4(targetBSphere),
                           glm::dmat4(modelMatrix), glm::dmat4(worldTransform));
  }

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
  void end();
};

#endif  // LOD_SHADOW_H_
