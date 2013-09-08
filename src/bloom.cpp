#include "bloom.h"

using namespace oglwrap;

BloomEffect::BloomEffect()
    : vs("bloom.vert")
    , fs("bloom.frag") {

    prog << vs << fs;
    prog.link().use();

    vao.bind();
    verts.bind();
    {
        GLfloat screenCorners[] = {
            -1.0f, -1.0f,
            +1.0f, -1.0f,
            -1.0f, +1.0f,
            +1.0f, +1.0f
        };
        verts.data(sizeof(screenCorners), screenCorners);
        (prog | "Position").setup<float>(2).enable();
    }
    vao.unbind();
}

void BloomEffect::reshape(GLuint w, GLuint h) {
    width = w;
    height = h;

    tex.active(0);
    tex.bind();
    tex.upload(
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
    tex.active(0);
    tex.bind();
    tex.copy(PixelDataInternalFormat::RGB, 0, 0, width, height);

    gl( Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

    prog.use();
    vao.bind();
    gl( DrawArrays(GL_TRIANGLE_STRIP, 0, 4) );
    vao.unbind();
}
