#ifndef LOD_SKYBOX_HPP_
#define LOD_SKYBOX_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/shapes/cube.hpp"
#include "oglwrap/textures/texture_cube.hpp"

class Skybox {
  oglwrap::Cube cube_;

  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_;
  oglwrap::LazyUniform<glm::mat3> uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  oglwrap::TextureCube env_map_;
  oglwrap::FragmentShader sky_fs_;
public:
  Skybox();
  void resize(const glm::mat4& projMat);
  void render(float time, const glm::mat4& camMatrix);

  const oglwrap::TextureCube& env_map;
  const oglwrap::FragmentShader& sky_fs;
  glm::vec3 getSunPos(float time) const;
  glm::vec4 getSunData(float time) const;
};


#endif // LOD_SKYBOX_HPP_
