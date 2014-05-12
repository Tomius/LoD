// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAMEOBJECT_H_
#define ENGINE_GAMEOBJECT_H_

#include <memory>
#include "camera.h"
#include "rigid_body.h"

namespace engine {

class GameObject {
public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  virtual ~GameObject() {}

  void addRigidBody(RigidBody::CallBack getTerrainHeight,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
      new RigidBody{transform, getTerrainHeight, starting_height}
    );
  }

  virtual void screenResized(const glm::mat4& projMat,
                             size_t width, size_t height) {}
  virtual void update(float t) {}
  virtual void shadowRender(float time, const Camera& cam) {}
  virtual void render(float time, const Camera& cam) {}
};

}


#endif
