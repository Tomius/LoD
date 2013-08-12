#ifndef HEADER_16753D7B23630095
#define HEADER_16753D7B23630095

#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/texture.hpp"
#include "oglwrap/shapes/cube.hpp"

class Skybox {
    oglwrap::Cube makeCube;

    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;
    oglwrap::Program prog;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix;
    oglwrap::LazyUniform<glm::vec4> sunData;
public:
    Skybox();
    void Reshape(const glm::mat4& projMat);
    void Render(float time, const glm::mat4& camMatrix);

    oglwrap::TextureCube envMap;
    oglwrap::FragmentShader sky_fs;
    glm::vec4 getSunData(float time) const;
};


#endif // header guard
