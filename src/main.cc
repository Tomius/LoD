// Copyright (c) 2014, Tamas Csala

#include "engine/game_engine.h"
#include "scenes/main_scene.h"
#include "scenes/gui_test_scene.h"
#include "scenes/bullet_basics_scene.h"

using engine::GameEngine;

int main() {
  try {
    GameEngine::InitContext();
    GameEngine::LoadScene<MainScene>();  // The real Land of Dreams scene
    //  GameEngine::LoadScene<GuiTestScene>();  // Just for testing GUI elements
    //  GameEngine::LoadScene<BulletBasicsScene>();  // Bullet basics
    GameEngine::Run();
  } catch(const std::exception& err) {
    std::cerr << err.what();
    GameEngine::Destroy();
    std::terminate();
  }
}
