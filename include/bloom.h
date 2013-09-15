#ifndef HEADER_89E445A7D2F5D160
#define HEADER_89E445A7D2F5D160

#if defined(__APPLE__)
    #include <OpenGL/glew.h>
#else
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
        #include <windows.h>
    #endif
    #include <GL/glew.h>
#endif

#include "oglwrap/oglwrap.hpp"

class BloomEffect {
    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;
    oglwrap::Program prog;

    oglwrap::TextureRect tex;

    oglwrap::VertexArray vao;
    oglwrap::ArrayBuffer verts;

    GLuint width, height;
public:
    BloomEffect();
    void reshape(GLuint width, GLuint height);
    void render();
};


#endif // header guard
