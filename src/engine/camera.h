// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include <cmath>
#include <iostream>

#include "timer.h"
#include "transform.h"
#include "rigid_body.h"

namespace engine {

/// The base class for all cameras (basically a Transform)
class Camera : public Transform {
public:
  virtual ~Camera() {}
  virtual void update(const Timer& timer) {}
  virtual void mouseMoved(const Timer& timer, double xpos, double ypos) {}
  virtual void mouseScrolled(const Timer& timer, double xoffset, double yoffset) {}
  virtual void mouseButtonPressed(const Timer& timer, int button,
                                  int action, int mods) {}
  virtual void keyAction(const Timer& timer, int key, int scancode,
                         int action, int mods) {}
};

class FreeFlyCamera : public Camera {
  glm::vec3 fwd_;
  GLFWwindow* window_;
  bool first_call_;

  /// Private constant numbers
  const float speed_per_sec_, cos_max_pitch_angle_, mouse_sensitivity_;
public:
  /// Creates the free-fly camera.
  /** @param pos - The position of the camera.
    * @param target - The position of the camera's target (what it is looking at).
    * @param speed_per_sec - Move speed in OpenGL units per second
    * @param mouse_sensitivity - The relative sensitivity to mouse movement. */
  FreeFlyCamera(GLFWwindow* window,
                const glm::vec3& pos,
                const glm::vec3& target = glm::vec3(),
                float speed_per_sec = 5.0f,
                float mouse_sensitivity = 1.0f)
    : window_(window)
    , first_call_(true)
    , speed_per_sec_(speed_per_sec)
    , cos_max_pitch_angle_(0.8)
    , mouse_sensitivity_(mouse_sensitivity) {

    this->pos(pos);
    forward(target - pos);
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
    return glm::lookAt(pos(), pos()+forward(), up());
  }

  /// Updates the camera's position and rotation.
  virtual void update(const Timer& timer) override {
    static glm::dvec2 prev_cursor_pos;
    glm::dvec2 cursor_pos;
    glfwGetCursorPos(window_, &cursor_pos.x, &cursor_pos.y);
    glm::dvec2 diff = cursor_pos - prev_cursor_pos;
    prev_cursor_pos = cursor_pos;

    // We get invalid diff values at the startup
    if (first_call_) {
      diff = glm::dvec2(0, 0);
      first_call_ = false;
    }

    // Mouse movement - update the coordinate system
    if (diff.x || diff.y) {
      float dx ( diff.x * mouse_sensitivity_ * timer.dt / 16 );
      float dy ( -diff.y * mouse_sensitivity_ * timer.dt / 16 );

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = glm::dot(up(), forward());
      if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      // Modify the forward vector
      forward(glm::normalize(forward() + right()*dx + up()*dy));
    }

    // Update the position
    float ds = timer.dt * speed_per_sec_;
    if(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
      localPos() += forward() * ds;
    }
    if(glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
      localPos() -= forward() * ds;
    }
    if(glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
      localPos() += right() * ds;
    }
    if(glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
      localPos() -= right() * ds;
    }
  }

}; // FreeFlyCamera

/**
 * @brief A simple camera class, that follows something that has a Transform.
 *
 * It can be controlled with mouse movement, and mouse wheel mouseScrolled.
 */
class ThirdPersonalCamera : public Camera {
  glm::vec3 fwd_;

  // We shouldn't interpolate at the first call.
  bool first_call_;

  // For mouseScrolled interpolation
  double curr_dist_mod_, dest_dist_mod_;

  // Private constant number
  const double initial_distance_, cos_max_pitch_angle_,
               mouse_sensitivity_, mouse_scroll_sensitivity_;

  // The camera should collide with the terrain.
  std::function<double(double, double)> getTerrainHeight_;

  GLFWwindow* window_;

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
   * @param mouse_scroll_sensitivity  The relative sensitivity to mouse mouseScrolled.
   */
  ThirdPersonalCamera(GLFWwindow* window,
                      Transform& target,
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
    , getTerrainHeight_(getTerrainHeight)
    , window_(window) {

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

private:
  virtual void update(const Timer& timer) override {
    static glm::dvec2 prev_cursor_pos;
    glm::dvec2 cursor_pos;
    glfwGetCursorPos(window_, &cursor_pos.x, &cursor_pos.y);
    glm::dvec2 diff = cursor_pos - prev_cursor_pos;
    prev_cursor_pos = cursor_pos;

    // We get invalid diff values at the startup
    if (first_call_) {
      diff = glm::dvec2(0, 0);
      first_call_ = false;
    }

    // Mouse movement - update the coordinate system
    if (diff.x || diff.y) {
      float dx ( diff.x * mouse_sensitivity_ * timer.dt / 16 );
      float dy ( -diff.y * mouse_sensitivity_ * timer.dt / 16 );

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = glm::dot(up(), forward());
      if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      // Modify the forward vector
      forward(glm::normalize(forward() + right()*dx + up()*dy));
    }

    // Update the position
    glm::dvec3 tpos(target().pos()), fwd(forward());
    glm::dvec3 pos(tpos - fwd*curr_dist_mod_*initial_distance_);

    constexpr double collision_offset = 1.2;

    if (distanceOverTerrain(pos) > collision_offset) {
      // Interpolate the mouseScrolled if there isn't any collision
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
      } while (collision_dist_mod > 0.001);

      double dist_over_terrain = fabs(collision_offset - distanceOverTerrain());
      if (1.5 * dist_over_terrain >
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
  virtual void mouseScrolled(const Timer&, double, double yoffset) override {
    dest_dist_mod_ -= yoffset / 4.0f * mouse_scroll_sensitivity_;
    if (dest_dist_mod_ < 0.25f) {
      dest_dist_mod_ = 0.25f;
    } else if (dest_dist_mod_ > 2.0f) {
      dest_dist_mod_ = 2.0f;
    }
  }

}; // ThirdPersonalCamera

} // namespace engine

#endif // ENGINE_CAMERA_H_

