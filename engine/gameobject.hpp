#ifndef ENGINE_GAMEOBJECT_HPP_
#define ENGINE_GAMEOBJECT_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

#include "camera.hpp"
#include "rigid_body.hpp"

namespace engine {

class GameObject {
public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  void addRigidBody(RigidBody::CallBack getTerrainHeight,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
      new RigidBody{transform, getTerrainHeight, starting_height}
    );
  }

  virtual void screenResized(const glm::mat4& projMat,
                             GLuint width, GLuint height) {}
  virtual void update(float t) {}
  virtual void shadowRender(float time, const Camera& cam) {}
  virtual void render(float time, const Camera& cam) {}
};

}


#endif
