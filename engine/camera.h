/** @file camera.hpp
    @brief Implements the camera classes.
*/

#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include <cmath>

#include "timer.h"
#include "transform.h"
#include "rigid_body.h"

namespace engine {

/// The base class for all cameras (basically a Transform)
class Camera : public Transform {
public:
  virtual ~Camera() {}
  virtual void update(const Timer& timer) = 0;
  virtual void scrolling(int mouse_wheel_ticks) {}
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
  double curr_dist_mod_, dest_dist_mod_;

  // Private constant number
  const double initial_distance_, cos_max_pitch_angle_,
              mouse_sensitivity_, mouse_scroll_sensitivity_;

  // The camera should collide with the terrain.
  std::function<double(double, double)> getTerrainHeight_;

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
                      RigidBody::CallBack getTerrainHeight,
                      double mouse_sensitivity = 1.0,
                      double mouse_scroll_sensitivity = 1.0)
    : first_call_(true)
    , curr_dist_mod_(1.0)
    , dest_dist_mod_(1.0)
    , initial_distance_(glm::length(target.pos() - position))
    , cos_max_pitch_angle_(0.8)
    , mouse_sensitivity_(mouse_sensitivity)
    , mouse_scroll_sensitivity_(mouse_scroll_sensitivity)
    , getTerrainHeight_(getTerrainHeight) {

    target.addChild(*this);
    pos(position);
    forward((target.pos()-position) / float(initial_distance_));
  }

  virtual ~ThirdPersonalCamera() {}

  Transform& target() const {
    return *getParent();
  }

  // The position is counted in a different way
  virtual const glm::vec3 pos() const override {
    return target().pos() - forward() * float(curr_dist_mod_*initial_distance_);
  }

  virtual void pos(const glm::vec3& new_pos) override {
    localPos(new_pos - target().pos() + forward() *
      float(curr_dist_mod_*initial_distance_));
  }

  // The forward value is cached
  virtual glm::vec3 forward() const override {
    return fwd_;
  }

  virtual void forward(const glm::vec3& new_fwd) override {
    fwd_ = new_fwd;
  }

  // We want the camera to always treat Y as up.
  virtual glm::vec3 up() const override {
    return glm::vec3(0, 1, 0);
  }

  virtual void up(const glm::vec3& new_up) override {}

  // The right vector is treated in a different way too.
  virtual glm::vec3 right() const override {
    return glm::cross(forward(), up());
  }

  virtual void right(const glm::vec3& new_right) override {
    forward(glm::cross(up(), new_right));
  }

  virtual glm::mat4 localToWorldMatrix() const override {
    return glm::lookAt(pos(), target().pos(), up());
  }

  /// Updates the camera's position and rotation.
  virtual void update(const Timer& timer) override {
    using namespace glm;

    static sf::Vector2i prev_loc;
    sf::Vector2i loc = sf::Mouse::getPosition();
    sf::Vector2i diff = loc - prev_loc;
    prev_loc = loc;

    // We get invalid diff values at the startup
    if (first_call_) {
      diff = sf::Vector2i(0, 0);
      first_call_ = false;
    }

    // Mouse movement - update the coordinate system
    if (diff.x || diff.y) {
      float dx ( diff.x * mouse_sensitivity_ * 0.0035 );
      float dy ( -diff.y * mouse_sensitivity_ * 0.0035 );

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = dot(up(), forward());
      if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      // Modify the forward vector
      forward(glm::normalize(forward() + right()*dx + up()*dy));
    }

    updateDistance(timer);
  }

private:
  void updateDistance(const Timer& timer) {
    glm::dvec3 tpos(target().pos()), fwd(forward());
    glm::dvec3 pos(tpos - fwd*curr_dist_mod_*initial_distance_);

    constexpr double collision_offset = 1.2;

    if (distanceOverTerrain(pos) > collision_offset) {
      // Interpolate the scrolling if there isn't any collision
      double dist_diff_mod = dest_dist_mod_ - curr_dist_mod_;
      double last_dist_mod = curr_dist_mod_;
      if (fabs(dist_diff_mod) > timer.dt * mouse_scroll_sensitivity_) {
        int sign = dist_diff_mod / fabs(dist_diff_mod);
        curr_dist_mod_ += sign * timer.dt * mouse_scroll_sensitivity_;
      }

      // Check if we have just made the camera collide with the terrain
      pos = tpos - fwd*curr_dist_mod_*initial_distance_;
      if (distanceOverTerrain(pos) < collision_offset) {
        // if we did, do the interpolation back
        curr_dist_mod_ = last_dist_mod;
      }
    } else {
      // If the camera collides the terrain, some magic is needed.
      double collision_dist_mod = curr_dist_mod_;
      do {
        double dist =  collision_dist_mod*initial_distance_;
        pos = tpos - fwd*dist;
        if (distanceOverTerrain(pos) > collision_offset) {
          break;
        } else {
          collision_dist_mod *= 0.99;
        }
      } while(collision_dist_mod > 0.001);

      double dist_over_terrain = fabs(collision_offset - distanceOverTerrain());
      if (2 * dist_over_terrain >
          fabs((collision_dist_mod - curr_dist_mod_)*initial_distance_)) {
        curr_dist_mod_ += 15.0*timer.dt*(collision_dist_mod - curr_dist_mod_);
      } else  {
        raiseDistanceOverTerrain(dist_over_terrain);
      }
    }
  }

  double distanceOverTerrain(const glm::dvec3& pos) const {
    return pos.y - getTerrainHeight_(pos.x, pos.z);
  }

  double distanceOverTerrain() const {
    glm::dvec3 tpos(target().pos()), fwd(forward());
    return distanceOverTerrain(tpos - fwd*curr_dist_mod_*initial_distance_);
  }

  void raiseDistanceOverTerrain(double diff) {
    glm::dvec3 fwd(forward());
    fwd_ = glm::normalize(fwd*curr_dist_mod_*initial_distance_ - glm::dvec3(0, diff, 0));
  }

public:
  /**
   * @brief Changes the distance in which the camera should follow the target.
   *
   * @param mouse_wheel_ticks   The number of ticks, the mouse wheel was scrolled.
   *                            This function expects a positive value for an
   *                            upward scroll.
   */
  virtual void scrolling(int mouse_wheel_ticks) override {
    dest_dist_mod_ -= mouse_wheel_ticks / 4.0f * mouse_scroll_sensitivity_;
    if (dest_dist_mod_ < 0.25f) {
      dest_dist_mod_ = 0.25f;
    } else if (dest_dist_mod_ > 2.0f) {
      dest_dist_mod_ = 2.0f;
    }
  }

}; // ThirdPersonalCamera

} // namespace engine

#endif // ENGINE_CAMERA_H_

