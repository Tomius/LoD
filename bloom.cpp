#include "bloom.h"
#include "misc.h"

using namespace oglplus;

BloomEffect::BloomEffect()
    : vs(ObjectDesc("Bloom"), File2Str("bloom.vert"))
    , fs(ObjectDesc("Bloom"), File2Str("bloom.frag"))
    , prog(ObjectDesc("Bloom")) {

    prog << vs << fs;
    prog.Link();

    vao.Bind();
    verts.Bind(Buffer::Target::Array);
    {
        GLfloat screenCorners[] = {
            -1.0f, -1.0f,
            +1.0f, -1.0f,
            -1.0f, +1.0f,
            +1.0f, +1.0f
        };
        Buffer::Data(Buffer::Target::Array, sizeof(screenCorners), screenCorners);
        (prog | "Position").Setup<Vec2f>().Enable();
    }
    verts.Unbind(Buffer::Target::Array);
    vao.Unbind();
}

void BloomEffect::Reshape(GLuint w, GLuint h) {
    width = w;
    height = h;

    Texture::Active(0);
    tex.Bind(Texture::Target::Rectangle);
    Texture::Image2D(
            Texture::Target::Rectangle,
            0,
            PixelDataInternalFormat::RGB,
            width,
            height,
            0,
            PixelDataFormat::RGB,
            PixelDataType::Float,
            nullptr
    );
}

void BloomEffect::Render() {
    // Copy the backbuffer to the texture that our shader can fetch.
    Texture::Active(0);
    tex.Bind(Texture::Target::Rectangle);
    Texture::CopyImage2D(
            Texture::Target::Rectangle,
            0,
            PixelDataInternalFormat::RGB,
            0, 0,
            width,
            height,
            0
    );

    gl.Clear().ColorBuffer().DepthBuffer();

    // And BLOOOM!
    prog.Use();
    vao.Bind();
    gl.DrawArrays(PrimitiveType::TriangleStrip, 0, 4);
    vao.Unbind();
}
