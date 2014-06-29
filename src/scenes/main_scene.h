// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SCENES_MAIN_SCENE_H_
#define LOD_SCENES_MAIN_SCENE_H_

#include "../engine/scene.h"
#include "../charmove.h"

class MainScene : public engine::Scene {
 public:
  MainScene();
  virtual float gravity() const override { return 18.0f; }
};

#endif
