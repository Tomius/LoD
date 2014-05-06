#ifndef LOD_BLOOM_HPP_
#define LOD_BLOOM_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

#include "oglwrap/shapes/fullScreenRect.hpp"
#include "oglwrap/textures/texture_2D.hpp"

#include "../engine/gameobject.hpp"

class BloomEffect : public engine::GameObject {
  oglwrap::VertexShader vs_;
  oglwrap::FragmentShader fs_;
  oglwrap::Program prog_;

  oglwrap::Texture2D tex_;
  oglwrap::LazyUniform<glm::vec2> uScreenSize_;
  oglwrap::FullScreenRectangle rect_;

  GLuint width_, height_;
public:
  BloomEffect();
  void screenResized(const glm::mat4&, GLuint width, GLuint height) override;
  void render(float, const engine::Camera&) override;
};


#endif // LOD_BLOOM_HPP_
