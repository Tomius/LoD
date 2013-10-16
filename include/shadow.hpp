#ifndef LOD_SHADOW_HPP_
#define LOD_SHADOW_HPP_

#include "oglwrap/oglwrap.hpp"

class Shadow {
    oglwrap::Texture2D_Array tex_;
    std::vector<oglwrap::Framebuffer> fbo_;

    size_t w_, h_, size_, curr_depth_;
public:
    Shadow(size_t shadowMapSize, size_t depth)
        : fbo_(depth), size_(shadowMapSize), curr_depth_(0) {
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
      tex_.wrapS(Wrap::ClampToEdge);
      tex_.wrapT(Wrap::ClampToEdge);
      tex_.compareFunc(Enums::CompFunc::LEqual);
      tex_.compareMode(CompMode::CompareRefToTexture);

      // Setup the FBOs
      for(int i = 0; i < depth; ++i) {
        fbo_[i].bind();
        fbo_[i].attachTextureLayer(FboAttachment::Depth, tex_, 0, i);
        gl(DrawBuffer(GL_NONE)); // No color output in the bound framebuffer, only depth.
        fbo_[i].validate();
      }

      oglwrap::Framebuffer::Unbind();
    }

    void resize(size_t width, size_t height) {
      w_ = width;
      h_ = height;
    }

    glm::mat4 projMat(float size) const {
      return glm::ortho<float>(-size, size, -size, size, 0, 2*size);
    }

    glm::mat4 camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const {
      return glm::lookAt(glm::vec3(targetBSphere) + glm::normalize(lightSrcPos) * targetBSphere.w,
                         glm::vec3(targetBSphere), glm::vec3(0, 1, 0));
    }

    glm::mat4 modelCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere, glm::mat4 modelMatrix) const {
      return projMat(targetBSphere.w) * camMat(lightSrcPos, targetBSphere) * modelMatrix;
    }

    glm::mat4 biasCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const {
      // [-1, 1] -> [0, 1] convert
      glm::mat4 biasMatrix(
          0.5, 0.0, 0.0, 0.0,
          0.0, 0.5, 0.0, 0.0,
          0.0, 0.0, 0.5, 0.0,
          0.5, 0.5, 0.5, 1.0
      );

      return biasMatrix * projMat(targetBSphere.w) * camMat(lightSrcPos, targetBSphere);
    }

    const oglwrap::Texture2D_Array& shadowTex() const {
      return tex_;
    }

    void begin() {
      fbo_[0].bind();
      glViewport(0, 0, size_, size_);
      glClear(GL_DEPTH_BUFFER_BIT);
    }

    void push() {
      fbo_[++curr_depth_].bind();
    }

    void end() {
      curr_depth_ = 0;
      oglwrap::Framebuffer::Unbind();
      glViewport(0, 0, w_, h_);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

#endif // LOD_SHADOW_HPP_
