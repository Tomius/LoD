#include "bloom.hpp"

using namespace oglwrap;

BloomEffect::BloomEffect()
  : vs_("bloom.vert")
  , fs_("bloom.frag")
  , uScreenSize_(prog_, "uScreenSize") {

  prog_ << vs_ << fs_;
  prog_.link().use();

  rect_.setupPositions(prog_ | "aPosition");

  tex_.active(0);
  tex_.bind();
  tex_.minFilter(MinFilter::Linear);
}

void BloomEffect::resize(GLuint w, GLuint h) {
  width_ = w;
  height_ = h;
  prog_.use();
  uScreenSize_ = glm::vec2(w, h);

  tex_.active(0);
  tex_.bind();
  tex_.upload(
    PixelDataInternalFormat::RGB,
    width_,
    height_,
    PixelDataFormat::RGB,
    PixelDataType::Float,
    nullptr
  );
}

void BloomEffect::render() {
  // Copy the backbuffer to the texture that our shader can fetch.
  tex_.active(0);
  tex_.bind();
  tex_.copy(PixelDataInternalFormat::RGB, 0, 0, width_, height_);

  gl(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  prog_.use();
  rect_.render();

  tex_.unbind();
}
