#include "shadow.hpp"

using namespace oglwrap;
extern Context gl;

Shadow::Shadow(size_t shadowMapSize, size_t depth)
  : fbo_(depth), cp_matrices_(depth), size_(shadowMapSize)
  , curr_depth_(0), max_depth_(depth) {
  using namespace oglwrap;

  // Setup the texture array that will serve as storage.
  tex_.bind();
  tex_.upload(
    PixelDataInternalFormat::DepthComponent,
    size_, size_, depth,
    PixelDataFormat::DepthComponent,
    PixelDataType::Float, nullptr
  );
  tex_.minFilter(MinFilter::Linear);
  tex_.magFilter(MagFilter::Linear);
  tex_.wrapS(Wrap::ClampToBorder);
  tex_.wrapT(Wrap::ClampToBorder);
  tex_.borderColor(glm::vec4(1.0f));
  tex_.compareFunc(Enums::CompFunc::LEqual);
  tex_.compareMode(CompMode::CompareRefToTexture);

  // Setup the FBOs
  for(int i = 0; i < depth; ++i) {
    fbo_[i].bind();
    fbo_[i].attachTextureLayer(FboAttachment::Depth, tex_, 0, i);
    // No color output in the bound framebuffer, only depth.
    gl.DrawBuffer(ColorBuffer::None);
    fbo_[i].validate();
  }

  Framebuffer::Unbind();
}

void Shadow::resize(size_t width, size_t height) {
  w_ = width;
  h_ = height;
}

glm::mat4 Shadow::projMat(float size) const {
  return glm::ortho<float>(-size, size, -size, size, 0, 2*size);
}

glm::mat4 Shadow::camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const {
  return glm::lookAt(glm::vec3(targetBSphere) + glm::normalize(lightSrcPos) * targetBSphere.w,
                     glm::vec3(targetBSphere), glm::vec3(0, 1, 0));
}

glm::mat4 Shadow::modelCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere,
                                  glm::mat4 modelMatrix, glm::mat4 worldTransform) {
  // [-1, 1] -> [0, 1] convert
  glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
  );

  glm::mat4 projMatrix = projMat(targetBSphere.w);
  glm::vec4 offseted_targetBSphere =
    glm::vec4(glm::vec3(modelMatrix * glm::vec4(glm::vec3(targetBSphere), 1)), targetBSphere.w);

  glm::mat4 pc = projMatrix * camMat(lightSrcPos, offseted_targetBSphere);

  cp_matrices_[curr_depth_] = biasMatrix * pc;

  return pc * modelMatrix * worldTransform;
}

const std::vector<glm::mat4>& Shadow::shadowCPs() const {
  return cp_matrices_;
}

const Texture2D_Array& Shadow::shadowTex() const {
  return tex_;
}

void Shadow::begin() {
  fbo_[0].bind();
  curr_depth_ = 0;
  gl.Viewport(size_, size_);
  gl.Clear().Depth();
}

void Shadow::push() {
  if(curr_depth_ + 1 < max_depth_) {
    fbo_[++curr_depth_].bind();
    gl.Clear().Depth();
  } else {
    throw std::overflow_error("ShadowMap stack overflow.");
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
