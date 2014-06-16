// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_ENGINE_H_
#define ENGINE_GAME_ENGINE_H_

#include <typeinfo>
#include "./scene.h"

namespace engine {

class GameEngine {
 public:
  // Initializes the OpenGL context
  static void InitContext();

  static void Destroy() {
    glfwDestroyWindow(window_);
    glfwTerminate();
    delete scene_;
  }

  // Replaces the current scene with a new one, of the specified type.
  template <typename Scene_t>
  static void LoadScene() {
    static_assert(std::is_base_of<Scene, Scene_t>::value,
                  "The given template type is not a Scene");

    try {
      scene_ = new Scene_t();
    } catch(const std::exception& err) {
      std::cerr << "Unable to load scene:\n" << err.what() << std::endl;
      std::cerr << "Stopping now." << std::endl;
      Destroy();
      std::terminate();
    }

    // Make sure that the objects in the scene are aware of the screen size.
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    ScreenResizeCallback(window_, width, height);
  }

  static const Scene& scene() {
    return *scene_;
  }

  static GLFWwindow* window() {
    return window_;
  }

  static glm::vec2 window_size() {
    int width, height;
    glfwGetWindowSize(GameEngine::window(), &width, &height);
    return glm::vec2(width, height);
  }

  static void Run();

 private:
  static Scene *scene_;
  static GLFWwindow *window_;

  // Callbacks
  static void ErrorCallback(int error, const char* message) {
    std::cerr << message;
  }

  static void KeyCallback(GLFWwindow* window, int key, int scancode,
                          int action, int mods);

  static void CharCallback(GLFWwindow* window, unsigned codepoint) {
    scene_->charTyped(codepoint);
  }

  static void ScreenResizeCallback(GLFWwindow* window, int width, int height) {
    gl::Viewport(width, height);
    scene_->screenResized(width, height);
  }

  static void MouseScrolledCallback(GLFWwindow* window, double xoffset,
                                    double yoffset) {
    scene_->mouseScrolled(xoffset, yoffset);
  }

  static void MouseButtonPressed(GLFWwindow* window, int button,
                                 int action, int mods) {
    scene_->mouseButtonPressed(button, action, mods);
  }

  static void MouseMoved(GLFWwindow* window,  double xpos, double ypos) {
    scene_->mouseMoved(xpos, ypos);
  }
};

}  // namespace engine

#endif
