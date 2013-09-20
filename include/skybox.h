#ifndef HEADER_16753D7B23630095
#define HEADER_16753D7B23630095

#if defined(__APPLE__)
    #include <OpenGL/glew.h>
#else
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
        #include <windows.h>
    #endif
    #include <GL/glew.h>
#endif

#include "oglwrap/oglwrap.hpp"
#include "oglwrap/shapes/cube.hpp"

class Skybox {
    oglwrap::Cube makeCube;

    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;
    oglwrap::Program prog;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix;
    oglwrap::LazyUniform<glm::mat3> cameraMatrix;
    oglwrap::LazyUniform<glm::vec4> sunData;
public:
    Skybox();
    void reshape(const glm::mat4& projMat);
    void render(float time, const glm::mat4& camMatrix);

    oglwrap::TextureCube envMap;
    oglwrap::FragmentShader sky_fs;
    glm::vec3 SunPos(float time) const;
    glm::vec4 getSunData(float time) const;
};


#endif // header guard
