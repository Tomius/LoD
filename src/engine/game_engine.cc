// Copyright (c) 2014, Tamas Csala

#include <string>

#include "./oglwrap_config.h"
#include <GLFW/glfw3.h>

#include "../oglwrap/smart_enums.h"
#include "./game_engine.h"

static double last_debug_time = 0;

static void PrintDebugText(const std::string& str) {
  std::cout << str << ": ";
}

static void PrintDebugTime() {
  double curr_time = glfwGetTime();
  std::cout << curr_time - last_debug_time << " ms" << std::endl;
  last_debug_time = curr_time;
}

static void GlInit() {
  gl::Enable(gl::kDepthTest);
  gl::Hint(gl::kTextureCompressionHint, gl::kFastest);
}

namespace engine {

Scene *GameEngine::scene_ = nullptr;
Scene *GameEngine::new_scene_ = nullptr;
GLFWwindow *GameEngine::window_ = nullptr;
ShaderManager *GameEngine::shader_manager_ = new ShaderManager{};

void GameEngine::InitContext() {
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
    window_ = glfwCreateWindow(1920, 1080, "Land of Dreams",
                               glfwGetPrimaryMonitor(), nullptr);
    if (!window_){
      // If it failed, try a 2.1 context
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

      window_ = glfwCreateWindow(1920, 1080, "Land of Dreams",
                                glfwGetPrimaryMonitor(), nullptr);

      // If that one fails too, we can't do much...
      if (!window_) {
        glfwTerminate();
        std::terminate();
      }
    }
  PrintDebugTime();

  // Check the created OpenGL context's version
  double ogl_version =
      glfwGetWindowAttrib(window_, GLFW_CONTEXT_VERSION_MAJOR) +
      glfwGetWindowAttrib(window_, GLFW_CONTEXT_VERSION_MINOR) / 10.0;
  std::cout << " - OpenGL version: "  << ogl_version << std::endl;
  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  std::cout << " - Resolution: "  << width << " x " << height << std::endl;

  if (ogl_version < 2.1) {
    std::cerr << "At least OpenGL version 2.1 is required to run this program\n";
    std::terminate();
  }

  glfwMakeContextCurrent(window_);

  // GLEW init
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "GlewInit error: " << glewGetErrorString(err) << std::endl;
    std::terminate();
  }
  gl::GetError();

  // No V-sync needed.
  glfwSwapInterval(0);

  GlInit();

  glfwSetKeyCallback(window_, KeyCallback);
  glfwSetCharCallback(window_, CharCallback);
  glfwSetFramebufferSizeCallback(window_, ScreenResizeCallback);
  glfwSetScrollCallback(window_, MouseScrolledCallback);
  glfwSetMouseButtonCallback(window_, MouseButtonPressed);
  glfwSetCursorPosCallback(window_, MouseMoved);
}

void GameEngine::Run() {
  while (!glfwWindowShouldClose(window_)) {
    if (new_scene_) {
      delete scene_;
      scene_ = new_scene_;
      new_scene_ = nullptr;
    }
    gl::Clear().Color().Depth();
    scene_->turn();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  Destroy();
}

void GameEngine::KeyCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      case GLFW_KEY_F11: {
        static bool fix_mouse = false;
        fix_mouse = !fix_mouse;

        if (fix_mouse) {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
      } break;
      default:
        break;
    }
  }

  scene_->keyActionAll(key, scancode, action, mods);
}

}  // namespace engine
