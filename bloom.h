#ifndef HEADER_89E445A7D2F5D160
#define HEADER_89E445A7D2F5D160

#include <GL/glew.h>
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>

class BloomEffect {
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;

    oglplus::Texture tex;

    oglplus::VertexArray vao;
    oglplus::Buffer verts;

    GLuint width, height;
public:
    BloomEffect();
    void Reshape(GLuint width, GLuint height);
    void Render();
};


#endif // header guard
