// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_BEHAVIOUR_H_
#define ENGINE_BEHAVIOUR_H_

#include "./game_object.h"

namespace engine {

class Behaviour : public GameObject {
 public:
  explicit Behaviour(Scene* scene) : GameObject(scene) {}
  virtual ~Behaviour() {}

  virtual void update(const Scene& scene) {}
  virtual void keyAction(const Scene& scene, int key, int scancode,
                         int action, int mods) {}
  virtual void charTyped(const Scene& scene, unsigned codepoint) {}
  virtual void mouseScrolled(const Scene& scene, double xoffset,
                             double yoffset) {}
  virtual void mouseButtonPressed(const Scene& scene, int button,
                                  int action, int mods) {}
  virtual void mouseMoved(const Scene& scene, double xpos, double ypos) {}

  virtual void updateAll(const Scene& scene) override;
  virtual void keyActionAll(const Scene& scene, int key, int scancode,
                            int action, int mods) override;
  virtual void charTypedAll(const Scene& scene, unsigned codepoint) override;
  virtual void mouseScrolledAll(const Scene& scene, double xoffset,
                                double yoffset) override;
  virtual void mouseButtonPressedAll(const Scene& scene, int button,
                                     int action, int mods) override;
  virtual void mouseMovedAll(const Scene& scene, double xpos,
                             double ypos) override;
};

}  // namespace engine

#endif
