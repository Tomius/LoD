// Copyright (c) 2014, Tamas Csala

#include "charmove.h"
#include <GLFW/glfw3.h>
#include "engine/game_engine.h"

CharacterMovement::CharacterMovement(
                  GLFWwindow* window,
                  engine::Transform& transform,
                  engine::RigidBody& rigid_body,
                  float horizontal_speed,
                  float rotationSpeed_PerSec)
  : transform_(transform)
  , rigid_body_(rigid_body)
  , curr_rot_(0)
  , dest_rot_(0)
  , rot_speed_(rotationSpeed_PerSec)
  , vert_speed_(0)
  , horiz_speed_(horizontal_speed)
  , horiz_speed_factor_(1.0f)
  , walking_(false)
  , jumping_(false)
  , flip_(false)
  , can_flip_(true)
  , transition_(false)
  , window_(window)
  , anim_(nullptr)
  , camera_(nullptr)
  , can_jump_functor_(nullptr)
  , can_flip_functor_(nullptr)
{ }

void CharacterMovement::handleSpacePressed() {
  if (!jumping_) {
    if (can_jump_functor_ == nullptr || can_jump_functor_()) {
      jumping_ = true;
      flip_ = false;
      vert_speed_ = 10.0f;
      horiz_speed_factor_ = 1.0f;
    }
  } else if (can_flip_) {
    if (can_flip_functor_ == nullptr || can_flip_functor_()) {
      can_flip_ = false;
      flip_ = true;
      vert_speed_ = 11.0f;
      horiz_speed_factor_ = 1.3f;
    }
  }
}

void CharacterMovement::update(float time) {
  const engine::Camera& cam = *camera_;
  glm::vec2 character_offset = anim_->offsetSinceLastFrame();

  static float prevTime = 0;
  float dt =  time - prevTime;
  prevTime = time;

  glm::ivec2 moveDir; // up and right is positive
  bool w = glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS;
  bool a = glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS;
  bool s = glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS;
  bool d = glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS;

  if (w && !s) {
    moveDir.y = 1;
  } else if (s && !w) {
    moveDir.y = -1;
  }

  if (d && !a) {
    moveDir.x = 1;
  } else if (a && !d) {
    moveDir.x = -1;
  }

  static glm::ivec2 lastMoveDir;
  bool lastWalking = walking_;
  walking_ = moveDir.x || moveDir.y;
  transition_ = transition_ || (walking_ != lastWalking) || (lastMoveDir != moveDir);
  lastMoveDir = moveDir;

  if (walking_) {
    glm::vec3 fwd = cam.forward();
    double cameraRot = -atan2(fwd.z, fwd.x);
    double moveRot = atan2(moveDir.y, moveDir.x); // +y is forward
    dest_rot_ = gl::ToDegree(cameraRot + moveRot);
    dest_rot_ = fmod(dest_rot_, 360);

    double diff = dest_rot_ - curr_rot_;
    double sign = diff / fabs(diff);

    // Take the shorter path.
    while (fabs(diff) > 180) {
      dest_rot_ -= sign * 360;

      diff = dest_rot_ - curr_rot_;
      sign = diff / fabs(diff);
    }

    if (transition_) {
      if (fabs(diff) > rot_speed_ / 20.0f) {
        curr_rot_ += sign * dt * rot_speed_;
        curr_rot_ = fmod(curr_rot_, 360);
      } else {
        transition_ = false;
      }
    } else {
      curr_rot_ = fmod(dest_rot_, 360);
    }
  }

  glm::mat4 rotation = glm::rotate(glm::mat4(), (float)fmod(curr_rot_, 360),
                                   glm::vec3(0,1,0));
  transform_.set_rot(glm::quat_cast(rotation));

  {
    auto pos = transform_.pos_proxy();

    pos += glm::mat3(rotation) *
           glm::vec3(character_offset.x, 0, character_offset.y);
    if (jumping_) {
      pos += glm::mat3(rotation) *
             glm::vec3(0, 0, horiz_speed_ * horiz_speed_factor_ * dt);
    }
  }

  rigid_body_.update();
  updateHeight(time);
}

void CharacterMovement::updateHeight(float time) {
  auto& pos = transform_.local_pos();

  static float prevTime = 0;
  float diff_time = time - prevTime;
  prevTime = time;

  while (diff_time > 0) {
    float time_step = 0.01f;
    float dt = std::min(time_step, diff_time);
    diff_time -= time_step;

    if (pos.y < 0 && jumping_ && vert_speed_ < 0) {
      jumping_ = false;
      flip_ = false;
      can_flip_ = true;
      pos.y = 0;
      return;
    }

    if (!jumping_) {
      const float offs = std::max(fabs(pos.y / 2.0f), 0.05) * dt * 20.0f;
      if (fabs(pos.y) > offs) {
        pos.y -= pos.y / fabs(pos.y) * offs;
      }
    } else {
      if (pos.y < 0) {
        pos.y += std::max(pos.y, vert_speed_) * dt;
      } else {
        pos.y += vert_speed_ * dt;
      }
      vert_speed_ -= dt * engine::GameEngine::scene().gravity();
    }
  }
}

bool CharacterMovement::isJumping() const {
  return jumping_;
}

bool CharacterMovement::isJumpingRise() const {
  return jumping_ && !flip_ && vert_speed_ > 0;
}

bool CharacterMovement::isJumpingFall() const {
  return jumping_ && !flip_ && vert_speed_ < 0;
}

bool CharacterMovement::isDoingFlip() const {
  return flip_;
}

bool CharacterMovement::isWalking() const {
  return walking_;
}

void CharacterMovement::setFlip(bool flip) {
  flip_ = flip;
}
