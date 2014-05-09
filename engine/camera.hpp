/** @file camera.hpp
    @brief Implements the camera classes.
*/

#ifndef ENGINE_CAMERA_HPP_
#define ENGINE_CAMERA_HPP_

#include <cmath>
#include "oglwrap/oglwrap.hpp"

#include "transform.hpp"
#include "timer.hpp"

namespace engine {

/// The base class for all cameras
class Camera : public Transform {
public:
  virtual void update(const Timer& timer) = 0;
  virtual void scrolling(int mouse_wheel_ticks) {}

  virtual const glm::vec3 pos() const { return Transform::pos(); }
  virtual void pos(const glm::vec3& new_pos) { return Transform::pos(new_pos); }

  virtual const glm::quat rot() const { return Transform::rot(); }
  virtual void rot(const glm::quat& new_rot) { return Transform::rot(new_rot); }

  virtual const glm::vec3 scale() const { return glm::vec3{1, 1, 1}; }
  virtual void scale(const glm::vec3&) { }

  virtual glm::vec3 forward() const { return Transform::forward(); }
  virtual void forward(const glm::vec3& new_fwd) { Transform::forward(new_fwd); }
  virtual glm::vec3 up() const { return Transform::up(); }
  virtual void up(const glm::vec3& new_up) { Transform::up(new_up); }
  virtual glm::vec3 right() const { return Transform::right(); }
  virtual void right(const glm::vec3& new_right) { Transform::right(new_right); }

  virtual glm::mat4 matrix() const {return Transform::matrix(); };
  operator glm::mat4() const { return matrix(); }
};

/**
 * @brief A simple camera class, that follows something that has a Transform.
 *
 * It can be controlled with mouse movement, and mouse wheel scrolling.
 */
class ThirdPersonalCamera : public Camera {
  glm::vec3 fwd_;

  // We shouldn't interpolate at the first call.
  bool first_call_;

  // For scrolling interpolation
  float curr_dist_mod_, dest_dist_mod_;

  // Private constant number
  const float initial_distance_, cos_max_pitch_angle_,
              mouse_sensitivity_, mouse_scroll_sensitivity_;
public:
  /**
   * @brief Creates the third-personal camera.
   *
   * @param target                    The transformation of the object, that the
   *                                  camera should follow
   * @param position                  The position of the camera.
   * @param target                    The position of the camera's target (what
   *                                  it is looking at).
   * @param mouse_sensitivity         The relative sensitivity to mouse movement.
   * @param mouse_scroll_sensitivity  The relative sensitivity to mouse scrolling.
   */
  ThirdPersonalCamera(Transform& target,
                      const glm::vec3& position,
                      float mouse_sensitivity = 1.0f,
                      float mouse_scroll_sensitivity = 1.0f)
    : first_call_(true)
    , curr_dist_mod_(1.0f)
    , dest_dist_mod_(1.0f)
    , initial_distance_(glm::length(target.pos() - position))
    , cos_max_pitch_angle_(0.95f)
    , mouse_sensitivity_(mouse_sensitivity)
    , mouse_scroll_sensitivity_(mouse_scroll_sensitivity) {

    target.addChild(*this);
    pos(position);
    forward((target.pos()-position) / initial_distance_);
  }

  Transform& target() const {
    return *getParent();
  }

  // The camera's localPos can be used as an offset for the target.
  glm::vec3 targetPos() const {
    return Transform::pos();
  }

  // The position is counted in a different way
  const glm::vec3 pos() const override {
    return targetPos() - forward() * curr_dist_mod_*initial_distance_;
  }

  void pos(const glm::vec3& new_pos) override {
    Transform::pos(new_pos + forward() * curr_dist_mod_*initial_distance_);
  }

  // The forward value is cached
  glm::vec3 forward() const override {
    return fwd_;
  }

  void forward(const glm::vec3& new_fwd) override {
    fwd_ = new_fwd;
  }

  // We want the camera to always treat Y as up.
  glm::vec3 up() const override {
    return glm::vec3(0, 1, 0);
  }

  void up(const glm::vec3& new_up) override {}

  // The right vector is treated in a different way too.
  glm::vec3 right() const override {
    return glm::cross(forward(), up());
  }

  void right(const glm::vec3& new_right) override {
    forward(glm::cross(up(), new_right));
  }

  glm::mat4 matrix() const override {
    return glm::lookAt(pos(), targetPos(), up());
  }

  /// Updates the camera's position and rotation.
  void update(const Timer& timer) {
    using namespace glm;

    static sf::Vector2i prev_loc;
    sf::Vector2i loc = sf::Mouse::getPosition();
    sf::Vector2i diff = loc - prev_loc;
    prev_loc = loc;

    // We get invalid diff values at the startup
    if(first_call_) {
      diff = sf::Vector2i(0, 0);
      first_call_ = false;
    }

    // Mouse movement - update the coordinate system
    if(diff.x || diff.y) {
      float dx ( diff.x * mouse_sensitivity_ * 0.0035f );
      float dy ( -diff.y * mouse_sensitivity_ * 0.0035f );

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = dot(up(), forward());
      if(dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if(dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      // Modify the forward vector
      forward(glm::normalize(forward() + right()*dx + up()*dy));
    }

    // Interpolate the scrolling
    float dist_diff_mod = dest_dist_mod_ - curr_dist_mod_;
    if(fabs(dist_diff_mod) > timer.dt * mouse_scroll_sensitivity_) {
      int sign = dist_diff_mod / fabs(dist_diff_mod);
      curr_dist_mod_ += sign * timer.dt * mouse_scroll_sensitivity_;
    }
  }

  /**
   * @brief Changes the distance in which the camera should follow the target.
   *
   * @param mouse_wheel_ticks   The number of ticks, the mouse wheel was scrolled.
   *                            This function expects a positive value for an
   *                            upward scroll.
   */
  void scrolling(int mouse_wheel_ticks) {
    dest_dist_mod_ -= mouse_wheel_ticks / 5.0f * mouse_scroll_sensitivity_;
    if(dest_dist_mod_ < 0.25f) {
      dest_dist_mod_ = 0.25f;
    } else if(dest_dist_mod_ > 2.0f) {
      dest_dist_mod_ = 2.0f;
    }
  }

}; // ThirdPersonalCamera

} // namespace engine

#endif // ENGINE_CAMERA_HPP_

