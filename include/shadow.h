#ifndef HEADER_D5425A3FCC088D16
#define HEADER_D5425A3FCC088D16

#ifndef SHADOW_HPP
#define SHADOW_HPP

#include "oglwrap/oglwrap.hpp"
#include "oglwrap/texture.hpp"

const int shadowMapSize = 1024;

/// This class is so simple, that I implement it in the header
class Shadow {
    oglwrap::Texture2D tex;
    oglwrap::FrameBuffer fbo;

    size_t w, h;
public:
    Shadow() {
        using namespace oglwrap;
        // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
        tex.Bind();
        tex.Upload(
            PxDIntForm::DepthComponent, shadowMapSize, shadowMapSize,
            PxDForm::DepthComponent, PxDType::Float, nullptr
        );
        tex.MinFilter(MinF::Linear);
        tex.MagFilter(MagF::Linear);
        tex.WrapS(Wrap::ClampToEdge);
        tex.WrapT(Wrap::ClampToEdge);
        tex.CompareFunc(CompFunc::LEqual);
        tex.CompareMode(CompMode::CompareRefToTexture);

        fbo.Bind();
        fbo.AttachTexture(FBO_Attachment::Depth, tex, 0);
        glDrawBuffer(GL_NONE); // No color output in the bound framebuffer, only depth.
        fbo.Validate();
        fbo.Unbind();
    }

    void Resize(size_t width, size_t height) {
        w = width;
        h = height;
    }

    glm::mat4 ProjMat() const {
        return glm::ortho<float>(-6000, 6000, -6000, 6000, -3000, 12000);
    }

    glm::mat4 CameraMat(glm::vec3 _lightDir) const {
        glm::vec3 lightDir = glm::normalize(_lightDir) * 8000.0f;
        return glm::lookAt(lightDir, glm::vec3(), glm::vec3(0, 1, 0));
    }

    glm::mat4 MVP(glm::vec3 lightDir) const {
        return ProjMat() * CameraMat(lightDir);
    }

    glm::mat4 BiasMVP(glm::vec3 lightDir) const {
        // [-1, 1] -> [0, 1] convert
        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        return biasMatrix * MVP(lightDir);
    }

    const oglwrap::Texture2D& ShadowTex() const {
        return tex;
    }

    void ShadowRender() {
        fbo.Bind();
        glViewport(0, 0, shadowMapSize, shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void Render() {
        fbo.Unbind();
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

#endif // SHADOW_HPP

#endif // header guard
