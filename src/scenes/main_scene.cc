// Copyright (c) 2014, Tamas Csala

#include "./main_scene.h"

#include <iostream>
#include <string>

#include "../engine/game_engine.h"
#include "../engine/shader_manager.h"

#include "../charmove.h"
#include "../skybox.h"
#include "../terrain.h"
#include "../bloom.h"
#include "../ayumi.h"
#include "../tree.h"
#include "../shadow.h"
#include "../fps_display.h"

#include "../loading_screen.h"

static double last_debug_time = 0;

static void PrintDebugText(const std::string& str) {
  std::cout << str << ": ";
}

static void PrintDebugTime() {
  double curr_time = glfwGetTime();
  std::cout << curr_time - last_debug_time << " ms" << std::endl;
  last_debug_time = curr_time;
}

MainScene::MainScene() {
  GLFWwindow* window = engine::GameEngine::window();

  // Disable cursor
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // The scene builds quite slow, put some picture for the user.
  last_debug_time = 0;
  PrintDebugText("Drawing the loading screen");
    LoadingScreen().render();
    glfwSwapBuffers(window);
  PrintDebugTime();

  PrintDebugText("Initializing the skybox");
    Skybox *skybox = addComponent<Skybox>();
  PrintDebugTime();

  PrintDebugText("Initializing the shadow maps");
    Shadow *shadow = addShadow(skybox, 2048, 2, 2);
  PrintDebugTime();

  PrintDebugText("Initializing the terrain");
    Terrain *terrain = addComponent<Terrain>();
    const engine::HeightMapInterface& height_map = terrain->height_map();
  PrintDebugTime();

  PrintDebugText("Initializing Ayumi");
    Ayumi *ayumi = addComponent<Ayumi>();
    ayumi->addRigidBody(height_map, ayumi->transform.pos().y);

    CharacterMovement *charmove = ayumi->addComponent<CharacterMovement>(
        ayumi->transform, *ayumi->rigid_body);
    ayumi->charmove(charmove);
  PrintDebugTime();

  charmove->setAnimation(&ayumi->getAnimation());

  engine::Transform& cam_offset = addGameObject()->transform;

  glm::vec2 center = height_map.center();
  ayumi->transform.local_pos() =
      glm::vec3 {center.x, height_map.heightAt(center.x, center.y), center.y};
  ayumi->transform.addChild(cam_offset);
  cam_offset.set_local_pos(ayumi->getMesh().bSphereCenter());

  engine::ThirdPersonalCamera *cam = addCamera<engine::ThirdPersonalCamera>(
      window, static_cast<float>(M_PI/3.0f), 1.0f, 3000.0f, cam_offset,
      cam_offset.pos() + glm::vec3(ayumi->getMesh().bSphereRadius() * 2),
      height_map, 1.5f);

  charmove->setCamera(cam);

  PrintDebugText("Initializing the trees");
    addComponent<Tree>(height_map);
  PrintDebugTime();

  PrintDebugText("Initializing the resources for the bloom effect");
    BloomEffect *bloom = addComponent<BloomEffect>();
  PrintDebugTime();

  shadow->set_default_fbo(bloom->fbo());

  addComponent<FpsDisplay>();
}
