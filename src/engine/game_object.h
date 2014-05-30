// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_H_
#define ENGINE_GAME_OBJECT_H_

#include <memory>
#include "camera.h"
#include "rigid_body.h"

namespace engine {

class GameObject {
public:
  Transform transform;
  std::unique_ptr<RigidBody> rigid_body;

  virtual ~GameObject() {}

  void addRigidBody(const HeightMapInterface& height_map,
                    double starting_height = NAN) {
    rigid_body = std::unique_ptr<RigidBody>(
      new RigidBody{transform, height_map, starting_height}
    );
  }

  virtual void update(float t) {}
  virtual void shadowRender(float time, const Camera& cam) {}
  virtual void render(float time, const Camera& cam) {}
  virtual void drawGui() {}

  virtual void screenResized(size_t width, size_t height) {}
  virtual void keyAction(const Timer& timer, int key, int scancode,
                                             int action, int mods) {}
  virtual void mouseScrolled(const Timer& timer, double xoffset,
                                                 double yoffset) {}
  virtual void mouseButtonPressed(const Timer& timer, int button,
                                  int action, int mods) {}
  virtual void mouseMoved(const Timer& timer, double xpos, double ypos) {}
};

}


#endif
