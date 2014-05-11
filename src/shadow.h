#ifndef LOD_SHADOW_H_
#define LOD_SHADOW_H_

#include "oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/framebuffer.h"
#include "engine/gameobject.h"

class Shadow {
    oglwrap::Texture2D tex_;
    oglwrap::Framebuffer fbo_;

    size_t w_, h_, size_;
    size_t xsize_, ysize_, curr_depth_, max_depth_;
    std::vector<glm::mat4> cp_matrices_;

public:
    Shadow(size_t shadow_map_size, size_t atlas_x_size, size_t atlas_y_size);
    void screenResized(size_t width, size_t height);
    glm::mat4 projMat(float size) const;
    glm::mat4 camMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere) const;
    glm::mat4 modelCamProjMat(glm::vec3 lightSrcPos, glm::vec4 targetBSphere,
                              glm::mat4 modelMatrix, glm::mat4 worldTransform = glm::mat4());
    const std::vector<glm::mat4>& shadowCPs() const;
    const oglwrap::Texture2D& shadowTex() const;
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

#endif // LOD_SHADOW_H_
