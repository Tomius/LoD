// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include <cmath>

#define GLM_FORCE_RADIANS
#include "../oglwrap/glm/glm/gtx/rotate_vector.hpp"

#include "./timer.h"
#include "./transform.h"
#include "./rigid_body.h"
#include "collision/frustum.h"

namespace engine {

/// The base class for all cameras
class Camera : public Transform {
  float fovy_, z_near_, z_far_, width_, height_;

 public:
  Camera(float fovy, float z_near, float z_far)
      : fovy_(fovy), z_near_(z_near), z_far_(z_far), width_(0), height_(0) {
  }
  virtual ~Camera() {}
  virtual void update(const Timer& timer) {}
  virtual void mouseMoved(const Timer& timer, double xpos, double ypos) {}
  virtual void mouseScrolled(const Timer& timer, double xoffset, double yoffset) {}
  virtual void mouseButtonPressed(const Timer& timer, int button,
                                  int action, int mods) {}
  virtual void keyAction(const Timer& timer, int key, int scancode,
                         int action, int mods) {}

  void screenResized(size_t width, size_t height) {
    width_ = width;
    height_ = height;
  }
  glm::mat4 projectionMatrix() const {
    return glm::perspectiveFov<float>(fovy_, width_, height_, z_near_, z_far_);
  }

  float fovx() const { return fovy_*width_/height_;}
  void set_fovx(float fovx) { fovy_ = fovx*height_/width_; }
  float fovy() const { return fovy_;}
  void set_fovy(float fovy) { fovy_ = fovy; }
  float z_near() const { return z_near_;}
  void set_z_near(float z_near) { z_near_ = z_near; }
  float z_far() const { return z_far_;}
  void set_z_far(float z_far) { z_far_ = z_far; }

  Frustum frustum() const {
    glm::mat4 m = projectionMatrix() * matrix();

    // REMEMBER: m[i][j] is j-th row, i-th column!!!

    return {{
      // left
     {m[0][3] + m[0][0],
      m[1][3] + m[1][0],
      m[2][3] + m[2][0],
      m[3][3] + m[3][0]},

      // right
     {m[0][3] - m[0][0],
      m[1][3] - m[1][0],
      m[2][3] - m[2][0],
      m[3][3] - m[3][0]},

      // top
     {m[0][3] - m[0][1],
      m[1][3] - m[1][1],
      m[2][3] - m[2][1],
      m[3][3] - m[3][1]},

      // bottom
     {m[0][3] + m[0][1],
      m[1][3] + m[1][1],
      m[2][3] + m[2][1],
      m[3][3] + m[3][1]},

      // near
     {m[0][2],
      m[1][2],
      m[2][2],
      m[3][2]},

      // far
     {m[0][3] - m[0][2],
      m[1][3] - m[1][2],
      m[2][3] - m[2][2],
      m[3][3] - m[3][2]}
    }};

    // Note: there's no need to normalize the plane parameters
  }

  bool isPointInsideFrustum(const glm::vec3& p) const {
    glm::mat4 mat = projectionMatrix() * matrix();
    glm::vec4 proj = mat * glm::vec4(p, 1);
    proj /= proj.w;

    return -1 < proj.x && proj.x < 1 && -1 < proj.y && proj.y < 1 &&
            0 < proj.z && proj.z < 1;
  }
};

class FreeFlyCamera : public Camera {
  GLFWwindow* window_;
  bool first_call_;

  const float speed_per_sec_, cos_max_pitch_angle_, mouse_sensitivity_;

 public:
  FreeFlyCamera(GLFWwindow* window,
                float fov,
                float z_near,
                float z_far,
                const glm::vec3& pos,
                const glm::vec3& target = glm::vec3(),
                float speed_per_sec = 5.0f,
                float mouse_sensitivity = 1.0f)
      : Camera(fov, z_near, z_far)
      , window_(window)
      , first_call_(true)
      , speed_per_sec_(speed_per_sec)
      , cos_max_pitch_angle_(0.95f)
      , mouse_sensitivity_(mouse_sensitivity) {
    set_pos(pos);
    set_forward(target - pos);
  }

  // We want the camera to always treat Y as up.
  virtual glm::vec3 up() const override {
    return glm::vec3(0, 1, 0);
  }

  virtual void set_up(const glm::vec3& new_up) override {}

  // The right vector is treated in a different way too.
  virtual glm::vec3 right() const override {
    return glm::cross(forward(), up());
  }

  virtual void set_right(const glm::vec3& new_right) override {
    set_forward(glm::cross(up(), new_right));
  }

  virtual glm::mat4 localToWorldMatrix() const override {
    return glm::lookAt(pos(), pos()+forward(), up());
  }

  // Updates the camera's position and rotation.
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
      float dx(diff.x * mouse_sensitivity_ * timer.dt / 4);
      float dy(-diff.y * mouse_sensitivity_ * timer.dt / 4);

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = glm::dot(up(), forward());
      if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      set_forward(forward() + right()*dx + up()*dy);
    }

    // Update the position
    float ds = timer.dt * speed_per_sec_;
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
      local_pos() += forward() * ds;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
      local_pos() -= forward() * ds;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
      local_pos() += right() * ds;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
      local_pos() -= right() * ds;
    }
  }
};  // FreeFlyCamera

/**
 * @brief A simple camera class, that follows something that has a Transform.
 *
 * It can be controlled with mouse movement, and mouse wheel mouseScrolled.
 */
class ThirdPersonalCamera : public Camera {
  // We shouldn't interpolate at the first call.
  bool first_call_;

  // For mouseScrolled interpolation
  float curr_dist_mod_, dest_dist_mod_;

  // Private constant number
  const float initial_distance_, cos_max_pitch_angle_,
               mouse_sensitivity_, mouse_scroll_sensitivity_;

  // The camera should collide with the terrain.
  const engine::HeightMapInterface& height_map_;

  GLFWwindow* window_;

 public:
  ThirdPersonalCamera(GLFWwindow* window,
                      float fov,
                      float z_near,
                      float z_far,
                      Transform& target,
                      const glm::vec3& position,
                      const engine::HeightMapInterface& height_map,
                      float mouse_sensitivity = 1.0f,
                      float mouse_scroll_sensitivity = 1.0f)
      : Camera(fov, z_near, z_far)
      , first_call_(true)
      , curr_dist_mod_(1.0f)
      , dest_dist_mod_(1.0f)
      , initial_distance_(glm::length(target.pos() - position))
      , cos_max_pitch_angle_(0.95f)
      , mouse_sensitivity_(mouse_sensitivity)
      , mouse_scroll_sensitivity_(mouse_scroll_sensitivity)
      , height_map_(height_map)
      , window_(window) {
    target.addChild(*this);
    set_pos(position);
    set_forward((target.pos()-position) / initial_distance_);
  }

  virtual ~ThirdPersonalCamera() {}

  Transform& target() const {
    return *getParent();
  }

  // The position is counted in a different way
  virtual const glm::vec3 pos() const override {
    return target().pos() - forward() * (curr_dist_mod_*initial_distance_);
  }

  virtual void set_pos(const glm::vec3& new_pos) override {
    set_local_pos(new_pos - target().pos() +
                  forward() * (curr_dist_mod_*initial_distance_));
  }

  // We want the camera to always treat Y as up.
  virtual glm::vec3 up() const override {
    return glm::vec3(0, 1, 0);
  }

  virtual void set_up(const glm::vec3& new_up) override {}

  // The right vector is treated in a different way too.
  virtual glm::vec3 right() const override {
    return glm::cross(forward(), up());
  }

  virtual void set_right(const glm::vec3& new_right) override {
    set_forward(glm::cross(up(), new_right));
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
      float dx(diff.x * mouse_sensitivity_ * timer.dt / 16);
      float dy(-diff.y * mouse_sensitivity_ * timer.dt / 16);

      // If we are looking up / down, we don't want to be able
      // to rotate to the other side
      float dot_up_fwd = glm::dot(up(), forward());
      if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
        dy = 0;
      }
      if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
        dy = 0;
      }

      set_forward(forward() + right()*dx + up()*dy);
    }

    // Update the position
    glm::vec3 tpos(target().pos()), fwd(forward());
    glm::vec3 pos(tpos - fwd*curr_dist_mod_*initial_distance_);

    constexpr float collision_offset = 1.2f;

    if (distanceOverTerrain(pos) > collision_offset) {
      // Interpolate the mouseScrolled if there isn't any collision
      float dist_diff_mod = dest_dist_mod_ - curr_dist_mod_;
      float last_dist_mod = curr_dist_mod_;
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
      float collision_dist_mod = curr_dist_mod_;
      do {
        float dist =  collision_dist_mod*initial_distance_;
        pos = tpos - fwd*dist;
        if (distanceOverTerrain(pos) > collision_offset) {
          break;
        } else {
          collision_dist_mod *= 0.99f;
        }
      } while (collision_dist_mod > 0.001f);

      float dist_over_terrain = fabs(collision_offset - distanceOverTerrain());
      if (1.5f * dist_over_terrain >
          fabs((collision_dist_mod - curr_dist_mod_)*initial_distance_)) {
        curr_dist_mod_ += 15.0f*timer.dt*(collision_dist_mod - curr_dist_mod_);
      } else  {
        raiseDistanceOverTerrain(dist_over_terrain);
      }
    }
  }

  double distanceOverTerrain(const glm::vec3& pos) const {
    return pos.y - height_map_.heightAt(pos.x, pos.z);
  }

  double distanceOverTerrain() const {
    glm::vec3 tpos(target().pos()), fwd(forward());
    return distanceOverTerrain(tpos - fwd*curr_dist_mod_*initial_distance_);
  }

  void raiseDistanceOverTerrain(double diff) {
    set_forward(glm::normalize(forward()*curr_dist_mod_*initial_distance_ -
                          glm::vec3(0, diff, 0)));
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
};  // ThirdPersonalCamera

}  // namespace engine

#endif  // ENGINE_CAMERA_H_
