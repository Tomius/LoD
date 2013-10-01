#ifndef LOD_SKYBOX_HPP_
#define LOD_SKYBOX_HPP_

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
  oglwrap::Cube make_cube_;

  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::LazyUniform<glm::mat4> projectionMatrix_;
  oglwrap::LazyUniform<glm::mat3> cameraMatrix_;
  oglwrap::LazyUniform<glm::vec4> sunData_;
public:
  Skybox();
  void reshape(const glm::mat4& projMat);
  void render(float time, const glm::mat4& camMatrix);

  oglwrap::TextureCube envMap;
  oglwrap::FragmentShader sky_fs;
  glm::vec3 SunPos(float time) const;
  glm::vec4 getSunData(float time) const;
};


#endif // LOD_SKYBOX_HPP_
