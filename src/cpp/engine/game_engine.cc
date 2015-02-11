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

    if (!glfwInit()) {
      std::terminate();
    }

    // Window creation
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
#if ENGINE_NO_FULLSCREEN
    window_ = glfwCreateWindow(vidmode->width, vidmode->height,
                               "Land of Dreams", nullptr, nullptr);
#else
    window_ = glfwCreateWindow(vidmode->width, vidmode->height,
                               "Land of Dreams", monitor, nullptr);
#endif

    if (!window_) {
      std::cerr << "FATAL: Couldn't create a glfw window. Aborting now." << std::endl;
      glfwTerminate();
      std::terminate();
    }
  PrintDebugTime();

  // Check the created OpenGL context's version
  int ogl_major_version = glfwGetWindowAttrib(window_, GLFW_CONTEXT_VERSION_MAJOR);
  int ogl_minor_version = glfwGetWindowAttrib(window_, GLFW_CONTEXT_VERSION_MINOR);
  std::cout << " - OpenGL version: "  << ogl_major_version << '.' << ogl_minor_version << std::endl;
  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  std::cout << " - Resolution: "  << width << " x " << height << std::endl;

  if (ogl_major_version < 2 || (ogl_major_version == 2 && ogl_minor_version < 1)) {
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
