#ifndef LOD_SHADOW_HPP_
#define LOD_SHADOW_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

class Shadow {
    oglwrap::Texture2D_Array tex_;
    std::vector<oglwrap::Framebuffer> fbo_;
    std::vector<glm::mat4> cp_matrices_;

    size_t w_, h_, size_, curr_depth_, max_depth_;
public:
    Shadow(size_t shadowMapSize, size_t depth);
    void resize(size_t width, size_t height);
    glm::mat4 projMat(float size) const;
    glm::mat4 camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const;
    glm::mat4 modelCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere, 
                              glm::mat4 modelMatrix, glm::mat4 worldTransform = glm::mat4());
    const std::vector<glm::mat4>& shadowCPs() const;
    const oglwrap::Texture2D_Array& shadowTex() const;
    void begin();
    void push();
    size_t getDepth() const;
    size_t getMaxDepth() const;
    void end();
};

#endif // LOD_SHADOW_HPP_
