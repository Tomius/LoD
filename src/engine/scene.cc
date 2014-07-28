// Copyright (c) 2014, Tamas Csala

#include "./scene.h"
#include "./game_engine.h"

namespace engine {

Scene::Scene()
    : Behaviour(nullptr)
    , physics_thread_should_quit_(false)
    , physics_thread_{[this](){
      while (true) {
        physics_can_run_.waitOne();
        if (physics_thread_should_quit_) { return; }
        updatePhysics();
        physics_finished_.set();
      }
    }}
    , camera_(nullptr), shadow_(nullptr), window_(GameEngine::window()) {
  set_scene(this);
}

ShaderManager* Scene::shader_manager() {
  return GameEngine::shader_manager();
}


}  // namespace engine
