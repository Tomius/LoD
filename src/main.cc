// Copyright (c) 2014, Tamas Csala

#include "engine/game_engine.h"
#include "scenes/main_scene.h"
#include "scenes/gui_test_scene.h"
#include "scenes/bullet_basics_scene.h"
//#include "scenes/bullet_height_field_scene.h"

using engine::GameEngine;

int main() {
  try {
    GameEngine::InitContext();
    GameEngine::LoadScene<MainScene>();
    // GameEngine::LoadScene<GuiTestScene>();
    // GameEngine::LoadScene<BulletBasicsScene>();
    // GameEngine::LoadScene<BulletHeightFieldScene>();
    GameEngine::Run();
  } catch(const std::exception& err) {
    std::cerr << err.what();
    GameEngine::Destroy();
    std::terminate();
  }
}
