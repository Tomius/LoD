// Copyright (c) 2014, Tamas Csala

#include <vector>
#include <string>
#include <iostream>
#include <functional>

#define OGLWRAP_INSTANTIATE 1
#include "./lod_oglwrap_config.h"

#include <GLFW/glfw3.h>

#include "engine/timer.h"
#include "engine/scene.h"
#include "engine/camera.h"
#include "engine/gameobject.h"
#include "engine/cdlod/terrain.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./terrain.h"
#include "./bloom.h"
#include "./ayumi.h"
#include "./tree.h"
#include "./shadow.h"
#include "./loading_screen.h"

using oglwrap::Capability;
using oglwrap::Face;
using gl = oglwrap::Context;

extern const float GRAVITY = 18.0f;
/* 0 -> max quality
   2 -> max performance */
extern const int PERFORMANCE = 1;
double ogl_version;
bool was_left_click = false;

void glInit(GLFWwindow* window) {
  gl::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  gl::ClearDepth(1.0f);
  gl::Enable(Capability::DepthTest);
  gl::Enable(Capability::DepthClamp);
  gl::CullFace(Face::Back);

  LoadingScreen().render();
  glfwSwapBuffers(window);
}

void FpsDisplay() {
  double time = glfwGetTime();

  static double accum_time = 0.0f;
  static double last_call = time;
  double dt = time - last_call;
  last_call = time;
  static int calls = 0;

  calls++;
  accum_time += dt;
  if (accum_time > 1.0f) {
    std::cout << "FPS: " << calls << std::endl;
    accum_time = calls = 0;
  }
}

static double last_debug_time = 0;

static void PrintDebugText(const std::string& str) {
  std::cout << str << ": ";
}

static void PrintDebugTime() {
  double curr_time = glfwGetTime();
  std::cout << curr_time - last_debug_time << " ms" << std::endl;
  last_debug_time = curr_time;
}

engine::Scene *scene;

// Callbacks
static void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode,
                                            int action, int mods) {
  if(action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      case GLFW_KEY_F11:
        static bool fix_mouse = false;
        fix_mouse = !fix_mouse;

        if (fix_mouse) {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        break;
      default:
        break;
    }
  }

  scene->keyAction(window, key, scancode, action, mods);
}

static void ScreenResizeCallback(GLFWwindow* window, int width, int height) {
  gl::Viewport(width, height);
  scene->screenResized(width, height);
}

static void MouseScrolledCallback(GLFWwindow* window, double xoffset,
                                                      double yoffset) {
  scene->mouseScrolled(window, xoffset, yoffset);
}

static void MouseButtonPressed(GLFWwindow* window, int button,
                               int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    was_left_click = true;
  }
  scene->mouseButtonPressed(window, button, action, mods);
}

static void MouseMoved(GLFWwindow* window,  double xpos, double ypos) {
  scene->mouseMoved(window, xpos, ypos);
}

int main() {
  PrintDebugText("Creating the OpenGL context");
    glfwSetErrorCallback(ErrorCallback);

    // GLFW init
    if (!glfwInit()) {
      std::terminate();
    }

    // First try to open a 3.3 context (for VertexAttribDivisor)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Window creation
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Land of Dreams",
                                          glfwGetPrimaryMonitor(), nullptr);
    if (!window){
      // If it failed, try a 2.1 context
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

      window = glfwCreateWindow(1920, 1080, "Land of Dreams",
                                glfwGetPrimaryMonitor(), nullptr);

      // If that one fails too, we can't do much...
      if (!window) {
        glfwTerminate();
        std::terminate();
      }
    }

    // Check the created OpenGL context's version
    ogl_version = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR) +
                  glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR) / 10.0;
    std::cout << " - OpenGL version: "  << ogl_version << std::endl;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    std::cout << " - Resolution: "  << width << " x " << height << std::endl;

    if (ogl_version < 2.1) {
      std::cout << "At least OpenGL version 2.1 is required to run this program\n";
      std::terminate();
    }

    // If it's ok, set it for the window
    glfwMakeContextCurrent(window);

    // GLEW init
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      std::cout << "GlewInit error: " << glewGetErrorString(err) << std::endl;
      return -1;
    }
    oglwrap::Context::GetError();
  PrintDebugTime();

  // No V-sync needed.
  glfwSwapInterval(0);

  try {
    glInit(window);

    scene = new engine::Scene{};

    PrintDebugText("Initializing the skybox");
      Skybox *skybox = scene->addSkybox();
    PrintDebugTime();

    PrintDebugText("Initializing the shadow maps");
      Shadow *shadow = scene->addShadow(512, 4, 4);
    PrintDebugTime();

    PrintDebugText("Initializing the terrain");
      Terrain *terrain = scene->addGameObject<Terrain>(skybox, shadow);
      const engine::HeightMapInterface& height_map = terrain->height_map();
    PrintDebugTime();

    PrintDebugText("Initializing the trees");
      scene->addGameObject<Tree>(height_map, skybox, shadow);
    PrintDebugTime();

    PrintDebugText("Initializing Ayumi");
      Ayumi *ayumi = scene->addGameObject<Ayumi>(window, skybox, shadow);
      ayumi->addRigidBody(height_map, ayumi->transform.pos().y);

      CharacterMovement charmove{window, ayumi->transform, *ayumi->rigid_body};
      ayumi->charmove(&charmove);
    PrintDebugTime();

    charmove.setAnimation(&ayumi->getAnimation());

    engine::Transform& cam_offset = scene->addGameObject()->transform;
    ayumi->transform.localPos() =
      glm::vec3{height_map.w()/2.0f, 13.0f, height_map.h()/2.0f};
    ayumi->transform.addChild(cam_offset);
    cam_offset.localPos(ayumi->getMesh().bSphereCenter());

    engine::ThirdPersonalCamera cam(window, 45, 0.5f, 6000.0f,
      cam_offset,
      cam_offset.pos() + glm::vec3(ayumi->getMesh().bSphereRadius() * 2),
      height_map, 1.5f);

    charmove.setCamera(&cam);
    scene->addCamera(&cam);

    PrintDebugText("Initializing the resources for the bloom effect");
      scene->addAfterEffect<BloomEffect>();
    PrintDebugTime();

    // Callbacks
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, ScreenResizeCallback);
    // This manual reset event is needed!
    ScreenResizeCallback(window, width, height);
    glfwSetScrollCallback(window, MouseScrolledCallback);
    glfwSetMouseButtonCallback(window, MouseButtonPressed);
    glfwSetCursorPosCallback(window, MouseMoved);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::cout << "\nStarting the main loop.\n" << std::endl;

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
      gl::Clear().Color().Depth();
      FpsDisplay();
      scene->turn();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    delete scene;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

  } catch(std::exception& err) {
    std::cerr << err.what();
    delete scene;
    glfwDestroyWindow(window);
    glfwTerminate();
    std::terminate();
  }
}
