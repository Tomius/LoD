#include "shadow.h"
#include "oglwrap/context.h"

using namespace oglwrap;
extern Context gl;

Shadow::Shadow(size_t shadow_map_size, size_t atlas_x_size, size_t atlas_y_size)
    : size_(shadow_map_size)
    , xsize_(atlas_x_size)
    , ysize_(atlas_y_size)
    , curr_depth_(0)
    , max_depth_(xsize_*ysize_)
    , cp_matrices_(max_depth_)  {
  using namespace oglwrap;

  // Setup the texture array that will serve as storage.
  tex_.bind();
  tex_.upload(
    PixelDataInternalFormat::DepthComponent,
    size_*xsize_, size_*ysize_,
    PixelDataFormat::DepthComponent,
    PixelDataType::Float, nullptr
  );
  tex_.minFilter(MinFilter::Nearest);
  tex_.magFilter(MagFilter::Nearest);
  tex_.wrapS(WrapMode::ClampToBorder);
  tex_.wrapT(WrapMode::ClampToBorder);
  tex_.borderColor(glm::vec4(1.0f));
  tex_.compareFunc(CompareFunc::LEqual);
  tex_.compareMode(CompareMode::CompareRefToTexture);

  // Setup the FBO
  fbo_.bind();
  fbo_.attachTexture(FramebufferAttachment::Depth, tex_, 0);
  // No color output in the bound framebuffer, only depth.
  gl.DrawBuffer(ColorBuffer::None);
  fbo_.validate();

  Framebuffer::Unbind();
}

void Shadow::screenResized(size_t width, size_t height) {
  w_ = width;
  h_ = height;
}

glm::mat4 Shadow::projMat(float size) const {
  return glm::ortho<float>(-size, size, -size, size, 0, 2*size);
}

glm::mat4 Shadow::camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const {
  return glm::lookAt(
    glm::vec3(targetBSphere) + glm::normalize(lightSrcPos) * targetBSphere.w,
    glm::vec3(targetBSphere),
    glm::vec3(0, 1, 0)
  );
}

glm::mat4 Shadow::modelCamProjMat(glm::vec3 lightSrcPos,
                                  glm::vec4 targetBSphere,
                                  glm::mat4 modelMatrix,
                                  glm::mat4 worldTransform) {
  // [-1, 1] -> [0, 1] convert
  glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
  );

  glm::mat4 projMatrix = projMat(targetBSphere.w);
  glm::vec4 offseted_targetBSphere =
    glm::vec4(
      glm::vec3(modelMatrix * glm::vec4(glm::vec3(targetBSphere), 1)),
      targetBSphere.w
    );

  glm::mat4 pc = projMatrix * camMat(lightSrcPos, offseted_targetBSphere);

  cp_matrices_[curr_depth_] = biasMatrix * pc;

  return pc * modelMatrix * worldTransform;
}

const std::vector<glm::mat4>& Shadow::shadowCPs() const {
  return cp_matrices_;
}

const Texture2D& Shadow::shadowTex() const {
  return tex_;
}

void Shadow::begin() {
  fbo_.bind();
  curr_depth_ = 0;

  // Clear the shadowmap atlas
  gl.Viewport(size_*xsize_, size_*ysize_);
  gl.Clear().Depth();

  // Setup the 0th shadowmap
  gl.Viewport(0, 0, size_, size_);
}

void Shadow::setViewPort() {
  size_t x = curr_depth_ / xsize_, y = curr_depth_ % xsize_;
  gl.Viewport(x*size_, y*size_, size_, size_);
}

void Shadow::push() {
  if (curr_depth_ + 1 < max_depth_) {
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
  Framebuffer::Unbind();
  gl.Viewport(w_, h_);
}
