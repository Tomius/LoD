#include "bloom.h"

using namespace oglwrap;

BloomEffect::BloomEffect()
    : vs("bloom.vert")
    , fs("bloom.frag") {

    prog << vs << fs;
    prog.Link();

    vao.Bind();
    verts.Bind();
    {
        GLfloat screenCorners[] = {
            -1.0f, -1.0f,
            +1.0f, -1.0f,
            -1.0f, +1.0f,
            +1.0f, +1.0f
        };
        verts.Data(sizeof(screenCorners), screenCorners);
        (prog | "Position").Setup<float>(2).Enable();
    }
    vao.Unbind();
}

void BloomEffect::Reshape(GLuint w, GLuint h) {
    width = w;
    height = h;

    tex.Active(0);
    tex.Bind();
    tex.Upload(
        PixelDataInternalFormat::RGB,
        width,
        height,
        PixelDataFormat::RGB,
        PixelDataType::Float,
        nullptr
    );
}

void BloomEffect::Render() {
    // Copy the backbuffer to the texture that our shader can fetch.
    tex.Active(0);
    tex.Bind();
    tex.Copy(PixelDataInternalFormat::RGB, 0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.Use();
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    vao.Unbind();
}
