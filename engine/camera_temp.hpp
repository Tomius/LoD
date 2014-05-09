/** @file camera.hpp
    @brief Implements a basic cameras, useful for testing.
*/

#ifndef ENGINE_CAMERA_HPP_
#define ENGINE_CAMERA_HPP_

#include <cmath>
#include <SFML/Window.hpp>
#include "oglwrap/oglwrap.hpp"

#include "transform.hpp"

namespace engine {

/// Purely virtual interface class for cameras.
class Camera {
public:
  Transform transform;

  /// Returns the camera's target.
  virtual glm::vec3 getTarget() const {
    return transform.pos() + transform.scale() * transform.forward();
  }
};


/// A simple camera class using SFML. Its position depends on an external target, usually a character.
/** It can be controlled with the WASD keys and the mouse */
class ThirdPersonalCamera : public Camera {
  // For scrolling interpolation
  glm::vec3 dest_scale_;

  // Don't interpolate at the first updateTarget call.
  bool first_call_;

  // Private constant number
  const float initial_distance_, max_pitch_angle_, mouse_sensitivity_, mouse_scroll_sensitivity_;
public:
  /// Creates the third-personal camera.
  /** @param pos - The position of the camera.
    * @param target - The position of the camera's target (what it is looking at).
    * @param mouse_sensitivity - The relative sensitivity to mouse movement.
    * @param mouse_scroll_sensitivity - The relative sensitivity to mouse scrolling. */
  ThirdPersonalCamera(const glm::vec3& pos,
                      const glm::vec3& target = glm::vec3(),
                      float mouse_sensitivity = 1.0f,
                      float mouse_scroll_sensitivity = 1.0f)
    : first_call_(true)
    , initial_distance_(glm::length(target - pos))
    , max_pitch_angle_(60./90. * M_PI_2)
    , mouse_sensitivity_(mouse_sensitivity)
    , mouse_scroll_sensitivity_(mouse_scroll_sensitivity) {

    transform.pos(pos);
  }

  /// Updates the camera's position and rotation.
  /** @param window - The currently active SFML window.
    * @param fix_mouse - Specifies if the mouse should be locked into the middle of the screen. */
  void updateRotation(const sf::Window& window, bool fix_mouse = false) {
    using namespace glm;

    static sf::Clock clock;
    static float prev_time = 0;
    float time = clock.getElapsedTime().asSeconds();
    float dt =  time - prev_time;
    prev_time = time;

    sf::Vector2i loc = sf::Mouse::getPosition(window);
    sf::Vector2i diff;
    if(fix_mouse) {
      sf::Vector2i screen_half = sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2);
      diff = loc - screen_half;
      sf::Mouse::setPosition(screen_half, window);
    } else {
      static sf::Vector2i prev_loc;
      diff = loc - prev_loc;
      prev_loc = loc;
    }

    // We get invalid diff values at the startup or if fix_mouse has just changed
    static bool first_exec = true, last_fix_mouse = fix_mouse;
    if(first_exec || last_fix_mouse != fix_mouse) {
      first_exec = false;
      last_fix_mouse = fix_mouse;
      diff = sf::Vector2i(0, 0);
    }

    // Mouse movement - update the coordinate system
    if(diff.x || diff.y) {
      roty_ += diff.x * mouse_sensitivity_ * 0.0035f;
      rotx_ += -diff.y * mouse_sensitivity_ * 0.0035f;

      // Positive rotx_ is down
      if(rotx_ < -max_pitch_angle_) {
        rotx_ = -max_pitch_angle_;
      }
      if(rotx_ > 0) {
        rotx_ = 0;
      }
    }

    float dist_diff_mod = dest_dist_mod_ - curr_dist_mod_;
    if(fabs(dist_diff_mod) > dt * mouse_scroll_sensitivity_) {
      int sign = dist_diff_mod / fabs(dist_diff_mod);
      curr_dist_mod_ += sign * dt * mouse_scroll_sensitivity_;
    }

    fwd_ = (initial_distance_ * curr_dist_mod_) * vec3(
      cos(rotx_) * cos(roty_),
      sin(rotx_),
      cos(rotx_) * sin(roty_)
    );
  }

  /// Changes the distance in which the camera should follow the target.
  /** @param mouse_wheel_ticks - The number of ticks, the mouse wheel was scrolled. Expect positive on up scroll. */
  void scrolling(int mouse_wheel_ticks) {
    if(mouse_wheel_ticks > 0 || !colliding_) {
      dest_dist_mod_ -= mouse_wheel_ticks / 5.0f * mouse_scroll_sensitivity_;
      if(dest_dist_mod_ < 0.25f) {
        dest_dist_mod_ = 0.25f;
      } else if(dest_dist_mod_ > 2.0f) {
        dest_dist_mod_ = 2.0f;
      }
    }
  }

  /// Updates the target of the camera. Is expected to be called every frame.
  /** @param target - the position of the object the camera should follow. */
  void updateTarget(const glm::vec3& target) {
    static sf::Clock clock;
    static float prev_time = 0;
    float time = clock.getElapsedTime().asSeconds();
    float diff_time = time - prev_time;
    prev_time = time;

    while(diff_time > 0) {
      float time_step = 0.01f;
      float dt = std::min(time_step, diff_time);
      diff_time -= time_step;

      if(first_call_) {
        target_ = target;
        first_call_ = false;
        return;
      }

      target_ = glm::vec3(target.x, target_.y, target.z);

      float diff = target.y - target_.y;
      const float offs = std::max(fabs(diff / 2.0f), 0.05) * dt * 20.0f;
      if(fabs(diff) > offs) {
        target_.y += diff / fabs(diff) * offs;
      }
    }
  }

  /// Returns the camera matrix.
  glm::mat4 cameraMatrix() const {
    return glm::lookAt(target_ - fwd_, target_, glm::vec3(0, 1, 0));
  }

  /// Returns the camera's target.
  glm::vec3 getTarget() const {
    return target_;
  }

  /// Returns the camera's position.
  glm::vec3 getPos() const {
    return target_ - fwd_;
  }

  glm::vec3 getDestPos() const {
    return target_ - glm::normalize(fwd_) * (initial_distance_ * curr_dist_mod_);
  }


  /// Returns the camera's CCW rotation angle around the X Euler axe.
  float getRotx() const {
    return rotx_;
  }

  /// Returns the camera's CCW rotation angle around the Y Euler axe.
  float getRoty() const {
    return roty_;
  }
}; // ThirdPersonalCamera

} // namespace engine

#endif // ENGINE_CAMERA_HPP_

