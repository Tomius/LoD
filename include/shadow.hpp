#ifndef LOD_SHADOW_HPP_
#define LOD_SHADOW_HPP_

#include "oglwrap/oglwrap.hpp"

const int shadowMapSize = 1024;

/// This class is so simple, that I implement it in the header
class Shadow {
    oglwrap::Texture2D tex;
    oglwrap::Framebuffer fbo;

    size_t w, h;
public:
    Shadow() {
        using namespace oglwrap;
        tex.bind();
        tex.upload(
            PixelDataInternalFormat::DepthComponent,
            shadowMapSize, shadowMapSize,
            PixelDataFormat::DepthComponent, PixelDataType::Float, nullptr
        );
        tex.minFilter(MinFilter::Linear);
        tex.magFilter(MagFilter::Linear);
        tex.wrapS(Wrap::ClampToEdge);
        tex.wrapT(Wrap::ClampToEdge);
        tex.compareFunc(CompFunc::LEqual);
        tex.compareMode(CompMode::CompareRefToTexture);

        fbo.bind();
        fbo.attachTexture(FboAttachment::Depth, tex, 0);
        gl(DrawBuffer(GL_NONE)); // No color output in the bound framebuffer, only depth.
        fbo.validate();
        fbo.unbind();
    }

    void resize(size_t width, size_t height) {
        w = width;
        h = height;
    }

    glm::mat4 projMat(float size) const {
        return glm::ortho<float>(-size, size, -size, size, 0, 2*size);
    }

    glm::mat4 cameraMat(glm::vec3 lightDir, glm::vec4 targetBSphere) const {
        return glm::lookAt(glm::vec3(targetBSphere) - glm::normalize(lightDir) * targetBSphere.w,
                           glm::vec3(targetBSphere), glm::vec3(0, 1, 0));
    }

    glm::mat4 projCamMat(glm::vec3 lightDir, glm::vec4 targetBSphere) const {
        return projMat(targetBSphere.w) * cameraMat(lightDir, targetBSphere);
    }

    glm::mat4 biasprojCamMat(glm::vec3 lightDir, glm::vec4 targetBSphere) const {
        // [-1, 1] -> [0, 1] convert
        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        return biasMatrix * projCamMat(lightDir, targetBSphere);
    }

    const oglwrap::Texture2D& ShadowTex() const {
        return tex;
    }

    void shadowRender() {
        fbo.bind();
        glViewport(0, 0, shadowMapSize, shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void endShadowRender() {
        fbo.unbind();
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

#endif // LOD_SHADOW_HPP_
