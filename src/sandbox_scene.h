// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SANDBOX_SCENE_H_
#define LOD_SANDBOX_SCENE_H_

#include <wchar.h>
#include "engine/scene.h"
#include "engine/gui/label.h"
#include "oglwrap/smart_enums.h"

struct SandboxScene : public engine::Scene {
  SandboxScene() {
    engine::Font font;
    font.set_size(30);
    addGameObject<engine::Label>(
      L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789",
      glm::vec2{-0.4f, 0.0f}, font);
  }
};

#endif
