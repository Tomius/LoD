// Copyright (c) 2014, Tamas Csala

#include "./camera.h"
#include "./scene.h"

namespace engine {

void FreeFlyCamera::update() {
  glm::dvec2 cursor_pos;
  GLFWwindow* window = scene_->window();
  glfwGetCursorPos(window, &cursor_pos.x, &cursor_pos.y);
  static glm::dvec2 prev_cursor_pos;
  glm::dvec2 diff = cursor_pos - prev_cursor_pos;
  prev_cursor_pos = cursor_pos;

  // We get invalid diff values at the startup
  if (first_call_) {
    diff = glm::dvec2(0, 0);
    first_call_ = false;
  }

  const float dt = scene_->camera_time().dt;

  // Mouse movement - update the coordinate system
  if (diff.x || diff.y) {
    float dx(diff.x * mouse_sensitivity_ * dt / 16);
    float dy(-diff.y * mouse_sensitivity_ * dt / 16);

    // If we are looking up / down, we don't want to be able
    // to rotate to the other side
    float dot_up_fwd = glm::dot(transform()->up(), transform()->forward());
    if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
      dy = 0;
    }
    if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
      dy = 0;
    }

    transform()->set_forward(transform()->forward() +
                             transform()->right()*dx +
                             transform()->up()*dy);
  }

  // Update the position
  float ds = dt * speed_per_sec_;
  glm::vec3 local_pos = transform()->local_pos();
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    local_pos += transform()->forward() * ds;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    local_pos -= transform()->forward() * ds;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    local_pos += transform()->right() * ds;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    local_pos -= transform()->right() * ds;
  }
  transform()->set_local_pos(local_pos);

  update_cache();
}

void ThirdPersonalCamera::update() {
  static glm::dvec2 prev_cursor_pos;
  glm::dvec2 cursor_pos;
  GLFWwindow* window = scene_->window();
  glfwGetCursorPos(window, &cursor_pos.x, &cursor_pos.y);
  glm::dvec2 diff = cursor_pos - prev_cursor_pos;
  prev_cursor_pos = cursor_pos;

  // We get invalid diff values at the startup
  if (first_call_) {
    diff = glm::dvec2(0, 0);
    first_call_ = false;
  }

  const float dt = scene_->camera_time().dt;

  // Mouse movement - update the coordinate system
  if (diff.x || diff.y) {
    float dx(diff.x * mouse_sensitivity_ * dt / 16);
    float dy(-diff.y * mouse_sensitivity_ * dt / 16);

    // If we are looking up / down, we don't want to be able
    // to rotate to the other side
    float dot_up_fwd = glm::dot(transform()->up(), transform()->forward());
    if (dot_up_fwd > cos_max_pitch_angle_ && dy > 0) {
      dy = 0;
    }
    if (dot_up_fwd < -cos_max_pitch_angle_ && dy < 0) {
      dy = 0;
    }

    transform()->set_forward(transform()->forward() +
                             transform()->right()*dx +
                             transform()->up()*dy);
  }

  // Update the position
  glm::vec3 tpos(target_->pos()), fwd(transform()->forward());
  glm::vec3 pos(tpos - fwd*curr_dist_mod_*initial_distance_);

  const float collision_offset = 1.2f;

  if (distanceOverTerrain(pos) > collision_offset) {
    // Interpolate the mouseScrolled if there isn't any collision
    float dist_diff_mod = dest_dist_mod_ - curr_dist_mod_;
    float last_dist_mod = curr_dist_mod_;
    if (fabs(dist_diff_mod) > dt * mouse_scroll_sensitivity_) {
      int sign = dist_diff_mod / fabs(dist_diff_mod);
      curr_dist_mod_ += sign * dt * mouse_scroll_sensitivity_;
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
      float dist = collision_dist_mod*initial_distance_;
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
      curr_dist_mod_ += 15.0f*dt*(collision_dist_mod - curr_dist_mod_);
    } else  {
      raiseDistanceOverTerrain(dist_over_terrain);
    }
  }

  fwd = transform()->forward();
  pos = tpos - fwd*curr_dist_mod_*initial_distance_;
  transform()->set_pos(pos);

  update_cache();
}

}  // namespace engine
