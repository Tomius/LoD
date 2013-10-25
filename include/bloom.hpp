#ifndef LOD_BLOOM_HPP_
#define LOD_BLOOM_HPP_

#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/shapes/fullScreenRect.hpp"

class BloomEffect {
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::TextureRect tex_;

  oglwrap::FullScreenRectangle rect_;

  GLuint width_, height_;
public:
  BloomEffect();
  void resize(GLuint width, GLuint height);
  void render();
};


#endif // LOD_BLOOM_HPP_
