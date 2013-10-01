#ifndef LOD_BLOOM_HPP_
#define LOD_BLOOM_HPP_

#if defined(__APPLE__)
#include <OpenGL/glew.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>
#endif

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
  void reshape(GLuint width_, GLuint height_);
  void render();
};


#endif // LOD_BLOOM_HPP_
