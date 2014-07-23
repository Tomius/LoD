// Copyright (c) 2014, Tamas Csala
//
//  _                    _          __   ____
// | |    __ _ _ __   __| |   ___  / _| |  _ \ _ __ ___  __ _ _ __ ___  ___
// | |   / _` | '_ \ / _` |  / _ \| |_  | | | | '__/ _ \/ _` | '_ ` _ \/ __|
// | |__| (_| | | | | (_| | | (_) |  _| | |_| | | |  __/ (_| | | | | | \__ \
// |_____\__,_|_| |_|\__,_|  \___/|_|   |____/|_|  \___|\__,_|_| |_| |_|___/


#include "engine/game_engine.h"
#include "scenes/main_scene.h"
#include "scenes/gui_test_scene.h"
#include "scenes/bullet_height_field_scene.h"

using engine::GameEngine;

int main(int argc, char* argv[]) {
  try {
    GameEngine::InitContext();
    // GameEngine::LoadScene<MainScene>();
    // GameEngine::LoadScene<GuiTestScene>();
    GameEngine::LoadScene<BulletHeightFieldScene>();
    GameEngine::Run();
  } catch(const std::exception& err) {
    std::cerr << err.what();
    GameEngine::Destroy();
    std::terminate();
  }
}
