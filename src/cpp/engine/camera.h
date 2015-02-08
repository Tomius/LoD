// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include <cmath>

#define GLM_FORCE_RADIANS
#include <glm/gtx/rotate_vector.hpp>

#include "./timer.h"
#include "./behaviour.h"
#include "./height_map_interface.h"
#include "collision/frustum.h"

namespace engine {

class CameraTransform : public Transform {
 public:
  CameraTransform() : up_(vec3{0, 1, 0}) {}

  // We shouldn't inherit the parent's rotation, like how a normal Transform does
  virtual const quat rot() const override { return rot_; }
  virtual void set_rot(const quat& new_rot) override { rot_ = new_rot; }

  // We have custom up and right vectors
  virtual vec3 up() const override { return up_; }
  virtual void set_up(const vec3& new_up) override { up_ = new_up; }
  virtual vec3 right() const override {
    return glm::cross(forward(), up());
  }

  virtual void set_right(const vec3& new_right) override {
    set_forward(glm::cross(up(), new_right));
  }

 private:
  vec3 up_;
};

/// The base class for all cameras
class Camera : public Behaviour {
 public:
  Camera(GameObject* parent, float fovy, float z_near, float z_far)
      : Behaviour(parent, CameraTransform{}), fovy_(fovy), z_near_(z_near)
      , z_far_(z_far), width_(0), height_(0) { }
  virtual ~Camera() {}

  virtual void screenResized(size_t width, size_t height) override {
    width_ = width;
    height_ = height;
  }

  const glm::mat4& cameraMatrix() const { return cam_mat_; }
  const glm::mat4& projectionMatrix() const { return proj_mat_; }
  const Frustum& frustum() const { return frustum_; }

  float fovx() const { return fovy_*width_/height_;}
  void set_fovx(float fovx) { fovy_ = fovx*height_/width_; }
  float fovy() const { return fovy_;}
  void set_fovy(float fovy) { fovy_ = fovy; }
  float z_near() const { return z_near_;}
  void set_z_near(float z_near) { z_near_ = z_near; }
  float z_far() const { return z_far_;}
  void set_z_far(float z_far) { z_far_ = z_far; }

  bool isPointInsideFrustum(const glm::vec3& p) const {
    glm::mat4 mat = projectionMatrix() * cameraMatrix();
    glm::vec4 proj = mat * glm::vec4(p, 1);
    proj /= proj.w;

    return -1 < proj.x && proj.x < 1 && -1 < proj.y && proj.y < 1 &&
            0 < proj.z && proj.z < 1;
  }

 protected:
  // it must be called through update()
  void update_cache() {
    updateCameraMatrix();
    updateProjectionMatrix();
    updateFrustum();
  }

 private:
  float fovy_, z_near_, z_far_, width_, height_;

  glm::mat4 cam_mat_, proj_mat_;
  Frustum frustum_;

  void updateCameraMatrix() {
    const Transform* t = transform();
    cam_mat_ = glm::lookAt(t->pos(), t->pos()+t->forward(), t->up());
  }

  void updateProjectionMatrix() {
    proj_mat_ = glm::perspectiveFov<float>(fovy_, width_, height_, z_near_, z_far_);
  }

  void updateFrustum() {
    glm::mat4 m = proj_mat_ * cam_mat_;

    // REMEMBER: m[i][j] is j-th row, i-th column!!!

    frustum_ = Frustum{{
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
};

class FreeFlyCamera : public Camera {
 public:
  FreeFlyCamera(GameObject* parent, float fov, float z_near,
                float z_far, const glm::vec3& pos,
                const glm::vec3& target = glm::vec3(),
                float speed_per_sec = 5.0f,
                float mouse_sensitivity = 1.0f)
      : Camera(parent, fov, z_near, z_far)
      , first_call_(true)
      , speed_per_sec_(speed_per_sec)
      , mouse_sensitivity_(mouse_sensitivity)
      , cos_max_pitch_angle_(0.98f) {
    transform()->set_pos(pos);
    transform()->set_forward(target - pos);
  }

 protected:
  bool first_call_;
  const float speed_per_sec_, mouse_sensitivity_, cos_max_pitch_angle_;

 private:
  virtual void update() override;
};

class ThirdPersonalCamera : public Camera {
 public:
  ThirdPersonalCamera(GameObject* parent,
                      float fov,
                      float z_near,
                      float z_far,
                      const glm::vec3& position,
                      const engine::HeightMapInterface& height_map,
                      float mouse_sensitivity = 1.0f,
                      float mouse_scroll_sensitivity = 1.0f)
      : Camera(parent, fov, z_near, z_far)
      , target_(parent->transform())
      , first_call_(true)
      , curr_dist_mod_(1.0f)
      , dest_dist_mod_(1.0f)
      , initial_distance_(glm::length(target_->pos() - position))
      , cos_max_pitch_angle_(0.98f)
      , mouse_sensitivity_(mouse_sensitivity)
      , mouse_scroll_sensitivity_(mouse_scroll_sensitivity)
      , height_map_(height_map) {
    transform()->set_pos(position);
    transform()->set_forward(target_->pos() - position);
  }

  virtual ~ThirdPersonalCamera() {}

 private:
    // The target object's transform, that the camera is following
  Transform *target_;

  // We shouldn't interpolate at the first call.
  bool first_call_;

  // For mouseScrolled interpolation
  float curr_dist_mod_, dest_dist_mod_;

  // Private constant number
  const float initial_distance_, cos_max_pitch_angle_,
               mouse_sensitivity_, mouse_scroll_sensitivity_;

  // The camera should collide with the terrain.
  const engine::HeightMapInterface& height_map_;

  virtual void update() override;

  double distanceOverTerrain(const glm::vec3& pos) const {
    return pos.y - height_map_.heightAt(pos.x, pos.z);
  }

  double distanceOverTerrain() const {
    glm::vec3 tpos(target_->pos()), fwd(transform()->forward());
    return distanceOverTerrain(tpos - fwd*curr_dist_mod_*initial_distance_);
  }

  void raiseDistanceOverTerrain(double diff) {
    transform()->set_forward(
        transform()->forward() * curr_dist_mod_ * initial_distance_ -
        glm::vec3(0, diff, 0));
  }

  virtual void mouseScrolled(double, double yoffset) override {
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
