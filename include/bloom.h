#ifndef HEADER_89E445A7D2F5D160
#define HEADER_89E445A7D2F5D160

#include <GL/glew.h>
#include "oglwrap.hpp"

class BloomEffect {
    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;
    oglwrap::Program prog;

    oglwrap::TextureRect tex;

    oglwrap::VertexArray vao;
    oglwrap::Buffer verts;

    GLuint width, height;
public:
    BloomEffect();
    void Reshape(GLuint width, GLuint height);
    void Render();
};


#endif // header guard
