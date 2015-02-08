// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_ENGINE_H_
#define ENGINE_GAME_ENGINE_H_

#include <typeinfo>
#include "./scene.h"

// #define ENGINE_NO_FULLSCREEN 1

namespace engine {

class ShaderManager;

class GameEngine {
 public:
  // Initializes the OpenGL context
  static void InitContext();

  static void Destroy() {
    delete scene_;
    delete new_scene_;
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  // Replaces the current scene with a new one, of the specified type.
  template <typename Scene_t>
  static void LoadScene() {
    static_assert(std::is_base_of<Scene, Scene_t>::value,
                  "The given template type is not a Scene");

    try {
      new_scene_ = new Scene_t();
    } catch(const std::exception& err) {
      std::cerr << "Unable to load scene:\n" << err.what() << std::endl;
      std::cerr << "Stopping now." << std::endl;
      Destroy();
      std::terminate();
    }
  }

  static Scene* scene() { return scene_; }

  static GLFWwindow* window() { return window_; }

  static ShaderManager* shader_manager() { return shader_manager_; }

  static glm::vec2 window_size() {
    int width, height;
    glfwGetWindowSize(window(), &width, &height);
    return glm::vec2(width, height);
  }

  static void Run();

 private:
  static Scene *scene_;
  static Scene *new_scene_;
  static GLFWwindow *window_;
  static ShaderManager *shader_manager_;

  // Callbacks
  static void ErrorCallback(int error, const char* message) {
    std::cerr << message;
  }

  static void KeyCallback(GLFWwindow* window, int key, int scancode,
                          int action, int mods);

  static void CharCallback(GLFWwindow* window, unsigned codepoint) {
    scene_->charTypedAll(codepoint);
  }

  static void ScreenResizeCallback(GLFWwindow* window, int width, int height) {
    gl::Viewport(width, height);
    scene_->screenResizedAll(width, height);
  }

  static void MouseScrolledCallback(GLFWwindow* window, double xoffset,
                                    double yoffset) {
    scene_->mouseScrolledAll(xoffset, yoffset);
  }

  static void MouseButtonPressed(GLFWwindow* window, int button,
                                 int action, int mods) {
    scene_->mouseButtonPressedAll(button, action, mods);
  }

  static void MouseMoved(GLFWwindow* window,  double xpos, double ypos) {
    scene_->mouseMovedAll(xpos, ypos);
  }
};

}  // namespace engine

#endif
