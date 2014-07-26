// Copyright (c) 2014, Tamas Csala

#include "./scene.h"
#include "./game_engine.h"

namespace engine {

Scene::Scene()
    : Behaviour(nullptr), camera_(nullptr), shadow_(nullptr)
    , window_(GameEngine::window()) {
  set_scene(this);
}

ShaderManager* Scene::shader_manager() {
  return GameEngine::shader_manager();
}


}  // namespace engine
