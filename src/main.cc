// Copyright (c) 2014, Tamas Csala

#include "engine/game_engine.h"
#include "./sandbox_scene.h"
#include "./mideu_scene.h"

using engine::GameEngine;

int main() {
  try {
    GameEngine::InitContext();
    //GameEngine::LoadScene<MideuScene>();
    GameEngine::LoadScene<SandboxScene>();
    GameEngine::Run();
  } catch(std::exception& err) {
    std::cerr << err.what();
    GameEngine::Destroy();
    std::terminate();
  }
}
