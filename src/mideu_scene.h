// Copyright (c) 2014, Tamas Csala

#ifndef LOD_MIDEU_SCENE_H_
#define LOD_MIDEU_SCENE_H_

#include "engine/scene.h"
#include "./charmove.h"

class MideuScene : public engine::Scene {
  std::unique_ptr<CharacterMovement> charmove_;
 public:
  MideuScene();
  virtual float gravity() override { return 18.0f; }
};

#endif
