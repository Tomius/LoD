// Copyright (c) 2014, Tamas Csala

#include "./scene.h"
#include "./game_engine.h"

namespace engine {

Scene::Scene() : Behaviour(nullptr), window_(GameEngine::window())
               , target_layer_(0) {
  set_scene(this);
}

}  // namespace engine
