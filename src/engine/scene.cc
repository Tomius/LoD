// Copyright (c) 2014, Tamas Csala

#include "./scene.h"
#include "./game_engine.h"

namespace engine {

Scene::Scene() : window_(GameEngine::window()), target_layer_(0) {}

}  // namespace engine
