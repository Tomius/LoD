// Copyright (c) 2014, Tamas Csala

#include <vector>
#include "./shadow.h"
#include "./skybox.h"
#include "oglwrap/context.h"
#include "oglwrap/smart_enums.h"

Shadow::Shadow(GameObject* parent, Skybox* skybox, int shadow_map_size,
               int atlas_x_size, int atlas_y_size)
    : GameObject(parent)
    , default_fbo_(nullptr)
    , w_(0), h_(0)
    , size_(shadow_map_size)
    , xsize_(atlas_x_size)
    , ysize_(atlas_y_size)
    , curr_depth_(0)
    , max_depth_(xsize_*ysize_)
    , cp_matrices_(max_depth_)
    , skybox_(skybox)  {
  gl::Bind(tex_);
  tex_.upload(gl::kDepthComponent, size_*xsize_, size_*ysize_,
              gl::kDepthComponent, gl::kFloat, nullptr);
  tex_.maxAnisotropy();
  tex_.minFilter(gl::kLinear);
  tex_.magFilter(gl::kLinear);
  tex_.wrapS(gl::kClampToBorder);
  tex_.wrapT(gl::kClampToBorder);
  tex_.borderColor(glm::vec4(1.0f));
  gl::Unbind(tex_);

  // Setup the FBO
  gl::Bind(fbo_);
  fbo_.attachTexture(gl::kDepthAttachment, tex_, 0);
  // No color output in the bound framebuffer, only depth.
  gl::DrawBuffer(gl::kNone);
  fbo_.validate();
  gl::Unbind(fbo_);
}

void Shadow::screenResized(size_t width, size_t height) {
  w_ = width;
  h_ = height;
}

glm::mat4 Shadow::projMat(float size) const {
  return glm::ortho<float>(-size, size, -size, size, 0, 2*size);
}

glm::mat4 Shadow::camMat(glm::vec3 lightSrcPos,
                         glm::vec4 targetBSphere) const {
  return glm::lookAt(
    glm::vec3(targetBSphere) + glm::normalize(lightSrcPos) * targetBSphere.w,
    glm::vec3(targetBSphere),
    glm::vec3(0, 1, 0));
}

glm::mat4 Shadow::modelCamProjMat(glm::vec4 targetBSphere,
                                  glm::mat4 modelMatrix,
                                  glm::mat4 worldTransform) {
  // [-1, 1] -> [0, 1] convert
  glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0);

  glm::mat4 projMatrix = projMat(targetBSphere.w);
  glm::vec4 offseted_targetBSphere =
    glm::vec4(glm::vec3(modelMatrix * glm::vec4(glm::vec3(targetBSphere), 1)),
              targetBSphere.w);

  glm::mat4 pc = projMatrix * camMat(skybox_->getLightSourcePos(),
                                     offseted_targetBSphere);

  cp_matrices_[curr_depth_] = biasMatrix * pc;

  return static_cast<glm::mat4>(pc * modelMatrix * worldTransform);
}

const std::vector<glm::mat4>& Shadow::shadowCPs() const {
  return cp_matrices_;
}

const gl::Texture2D& Shadow::shadowTex() const {
  return tex_;
}

void Shadow::begin() {
  gl::Bind(fbo_);
  curr_depth_ = 0;

  // Clear the shadowmap atlas
  gl::Clear().Depth();

  // Setup the 0th shadowmap
  gl::Viewport(0, 0, size_, size_);
}

void Shadow::setViewPort() {
  size_t x = curr_depth_ / xsize_, y = curr_depth_ % xsize_;
  gl::Viewport(x*size_, y*size_, size_, size_);
}

void Shadow::push() {
  if (curr_depth_ < max_depth_) {
    ++curr_depth_;
    setViewPort();
  }
}

size_t Shadow::getDepth() const {
  return curr_depth_;
}

size_t Shadow::getMaxDepth() const {
  return max_depth_;
}

void Shadow::end() {
  if (default_fbo_) {
    gl::Bind(*default_fbo_);
  } else {
    gl::Unbind(fbo_);
  }
  gl::Viewport(w_, h_);
}
