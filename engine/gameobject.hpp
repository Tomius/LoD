#ifndef ENGINE_GAMEOBJECT_HPP_
#define ENGINE_GAMEOBJECT_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

#include "camera.hpp"

#include "time.hpp"

namespace engine {

class GameObject {
public:
  virtual void screenResized(const glm::mat4& projMat,
                             GLuint width, GLuint height) {}
  virtual void update(float t) {}
  virtual void shadowRender(float time, const Camera& cam) {}
  virtual void render(float time, const Camera& cam) {}
};

}


#endif
