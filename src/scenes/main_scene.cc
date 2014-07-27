// Copyright (c) 2014, Tamas Csala

#include "./main_scene.h"

#include <iostream>
#include <string>

#include "../engine/rigid_body.h"
#include "../engine/game_engine.h"
#include "../engine/shader_manager.h"

#include "../charmove.h"
#include "../skybox.h"
#include "../terrain.h"
#include "../after_effects.h"
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
  std::cout << curr_time - last_debug_time << " s" << std::endl;
  last_debug_time = curr_time;
}

MainScene::MainScene() {
  GLFWwindow* window = this->window();

  // Disable cursor
#if !ENGINE_NO_FULLSCREEN
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

  // The scene builds quite slow, put some picture for the user.
  last_debug_time = glfwGetTime();
  PrintDebugText("Drawing the loading screen");
    LoadingScreen().render();
    glfwSwapBuffers(window);
  PrintDebugTime();

  PrintDebugText("Initializing the skybox");
    Skybox *skybox = addComponent<Skybox>();
    skybox->set_group(-1);
  PrintDebugTime();

  PrintDebugText("Initializing the shadow maps");
    Shadow *shadow = addComponent<Shadow>(skybox, 2048, 2, 2);
    set_shadow(shadow);
  PrintDebugTime();

  Terrain *terrain = engine::GameEngine::scene()->findComponent<Terrain>();
  if (!stealComponent(terrain)) {
    PrintDebugText("Initializing the terrain");
     terrain = addComponent<Terrain>();
    PrintDebugTime();
  }
  const engine::HeightMapInterface& height_map = terrain->height_map();

  PrintDebugText("Initializing Ayumi");
    Ayumi *ayumi = addComponent<Ayumi>();
    ayumi->addComponent<engine::RigidBody>(ayumi->transform(), height_map, 0);

    CharacterMovement *charmove = ayumi->addComponent<CharacterMovement>();
    ayumi->charmove(charmove);
    charmove->setAnimation(&ayumi->getAnimation());
  PrintDebugTime();

  PrintDebugText("Initializing the camera");
    GameObject* cam_offset_go = ayumi->addComponent<GameObject>();
    engine::Transform *cam_offset = cam_offset_go->transform();

    glm::vec2 center = height_map.center();
    ayumi->transform()->local_pos() =
        glm::vec3 {center.x, height_map.heightAt(center.x, center.y), center.y};
    cam_offset->set_local_pos(ayumi->getMesh().bSphereCenter());

    engine::ThirdPersonalCamera *cam =
      cam_offset_go->addComponent<engine::ThirdPersonalCamera>(
        M_PI/3.0f, 1.0f, 3000.0f,
        cam_offset->pos() + glm::vec3(ayumi->getMesh().bSphereRadius() * 2),
        height_map, 1.5f);

    set_camera(cam);
    charmove->setCamera(cam);
  PrintDebugTime();

  PrintDebugText("Initializing the trees");
    addComponent<Tree>(height_map);
  PrintDebugTime();

  PrintDebugText("Initializing the resources for the after effects");
    AfterEffects *after_effects = addComponent<AfterEffects>(skybox);
    shadow->set_default_fbo(after_effects->fbo());
    after_effects->set_group(1);
  PrintDebugTime();

  PrintDebugText("Initializing the FPS display");
    auto fps = addComponent<FpsDisplay>();
    fps->set_group(2);
  PrintDebugTime();
}
