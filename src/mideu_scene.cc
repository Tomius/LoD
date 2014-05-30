// Copyright (c) 2014, Tamas Csala

#include "mideu_scene.h"
#include <iostream>

#include "engine/game_engine.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./terrain.h"
#include "./bloom.h"
#include "./ayumi.h"
#include "./tree.h"
#include "./shadow.h"
#include "./fps_display.h"

#include "./loading_screen.h"

static double last_debug_time = 0;

static void PrintDebugText(const std::string& str) {
  std::cout << str << ": ";
}

static void PrintDebugTime() {
  double curr_time = glfwGetTime();
  std::cout << curr_time - last_debug_time << " ms" << std::endl;
  last_debug_time = curr_time;
}

MideuScene::MideuScene() {
  GLFWwindow* window = engine::GameEngine::window();

  // The scene builds quite slow, put some picture for the user.
  LoadingScreen().render();
  glfwSwapBuffers(window);

  PrintDebugText("Initializing the skybox");
    Skybox *skybox = addSkybox();
  PrintDebugTime();

  PrintDebugText("Initializing the shadow maps");
    Shadow *shadow = addShadow(512, 4, 4);
  PrintDebugTime();

  PrintDebugText("Initializing the terrain");
    Terrain *terrain = addGameObject<Terrain>(skybox, shadow);
    const engine::HeightMapInterface& height_map = terrain->height_map();
  PrintDebugTime();

  PrintDebugText("Initializing the trees");
    addGameObject<Tree>(height_map, skybox, shadow);
  PrintDebugTime();

  PrintDebugText("Initializing Ayumi");
    Ayumi *ayumi = addGameObject<Ayumi>(window, skybox, shadow);
    ayumi->addRigidBody(height_map, ayumi->transform.pos().y);

    charmove_ = make_unique<CharacterMovement>(window, ayumi->transform,
                                               *ayumi->rigid_body);
    ayumi->charmove(charmove_.get());
  PrintDebugTime();

  charmove_->setAnimation(&ayumi->getAnimation());

  engine::Transform& cam_offset = addGameObject()->transform;
  ayumi->transform.localPos() =
    glm::vec3{height_map.w()/2.0f, 13.0f, height_map.h()/2.0f};
  ayumi->transform.addChild(cam_offset);
  cam_offset.localPos(ayumi->getMesh().bSphereCenter());

  engine::ThirdPersonalCamera *cam = addCamera<engine::ThirdPersonalCamera>(
    window, 45.0f, 0.5f, 6000.0f, cam_offset,
    cam_offset.pos() + glm::vec3(ayumi->getMesh().bSphereRadius() * 2),
    height_map, 1.5f);

  charmove_->setCamera(cam);

  PrintDebugText("Initializing the resources for the bloom effect");
    addGameObject<BloomEffect>();
  PrintDebugTime();

  addGameObject<FpsDisplay>();
}
