#include "charmove.hpp"

#define SPEED_HACK 1.0f

using namespace oglwrap;

CharacterMovement::CharacterMovement(glm::vec3 pos,
                                     float horizontal_speed,
                                     float rotationSpeed_PerSec)
  : pos_(pos)
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
  , can_jump_functor_(nullptr)
  , can_flip_functor_(nullptr)
{ }

void CharacterMovement::handleSpacePressed() {
  if(!jumping_) {
    if(can_jump_functor_ == nullptr || can_jump_functor_()) {
      jumping_ = true;
      flip_ = false;
      vert_speed_ = 10.0f;
      horiz_speed_factor_ = 1.0f;
    }
  } else if (can_flip_) {
    if(can_flip_functor_ == nullptr || can_flip_functor_()) {
      can_flip_ = false;
      flip_ = true;
      vert_speed_ = 11.0f;
      horiz_speed_factor_ = 1.3f;
    }
  }
}

void CharacterMovement::update(const Camera& cam, glm::vec2 character_offset) {
  using namespace glm;

  static sf::Clock clock;
  static float prevTime = 0;
  float time = clock.getElapsedTime().asSeconds();
  float dt =  time - prevTime;
  prevTime = time;

  ivec2 moveDir; // up and right is positive
  bool w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
  bool a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
  bool s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
  bool d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

  if(w && !s) {
    moveDir.y = 1;
  } else if(s && !w) {
    moveDir.y = -1;
  }

  if(d && !a) {
    moveDir.x = 1;
  } else if(a && !d) {
    moveDir.x = -1;
  }

  static ivec2 lastMoveDir;
  bool lastWalking = walking_;
  walking_ = moveDir.x || moveDir.y;
  transition_ = transition_ || (walking_ != lastWalking) || (lastMoveDir != moveDir);
  lastMoveDir = moveDir;

  if(walking_) {
    double cameraRot = -cam.getRoty(); // -z is forward
    double moveRot = atan2(moveDir.y, moveDir.x); // +y is forward
    dest_rot_ = ToDegree(cameraRot + moveRot);
    dest_rot_ = fmod(dest_rot_, 360);

    double diff = dest_rot_ - curr_rot_;
    double sign = diff / fabs(diff);

    // Take the shorter path.
    while(fabs(diff) > 180) {
      dest_rot_ -= sign * 360;

      diff = dest_rot_ - curr_rot_;
      sign = diff / fabs(diff);
    }

    if(transition_) {
      if(fabs(diff) > rot_speed_ / 20.0f) {
        curr_rot_ += sign * dt * rot_speed_;
        curr_rot_ = fmod(curr_rot_, 360);
      } else {
        transition_ = false;
      }
    } else {
      curr_rot_ = fmod(dest_rot_, 360);
    }
  }

  mat3 transformation = mat3(rotate(mat4(), (float)fmod(curr_rot_, 360), vec3(0,1,0)));

  pos_ += transformation * SPEED_HACK * vec3(character_offset.x, 0, character_offset.y);
  if(jumping_) {
    pos_ += transformation * SPEED_HACK * vec3(0, 0, horiz_speed_ * horiz_speed_factor_ * dt);
  }
}

void CharacterMovement::updateHeight(float groundHeight) {
  static sf::Clock clock;
  static float prevTime = 0;
  float time = clock.getElapsedTime().asSeconds();
  float diff_time = time - prevTime;
  prevTime = time;

  while(diff_time > 0) {
    float time_step = 0.01f;
    float dt = std::min(time_step, diff_time);
    diff_time -= time_step;

    const float diff = groundHeight - pos_.y;
    if(diff >= 0 && jumping_ && vert_speed_ < 0) {
      jumping_ = false;
      flip_ = false;
      can_flip_ = true;
      pos_.y = groundHeight;
      return;
    }
    if(!jumping_) {
      const float offs = std::max(fabs(diff / 2.0f), 0.05) * dt * 20.0f;
      if(fabs(diff) > offs) {
        pos_.y += diff / fabs(diff) * offs;
      }
    }
    if(jumping_) {
      if(diff > 0) {
        pos_.y += std::max(diff, vert_speed_) * dt;
      } else {
        pos_.y += vert_speed_ * dt;
      }
      vert_speed_ -= dt * GRAVITY;
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

glm::mat4 CharacterMovement::getModelMatrix() const {
  glm::mat4 rot = glm::rotate(glm::mat4(), (float)fmod(curr_rot_, 360), glm::vec3(0,1,0));
  // The matrix's last column is responsible for the translation.
  rot[3] = glm::vec4(pos_, 1.0);
  return rot;
}

glm::vec3 CharacterMovement::getPos() const {
  return pos_;
}
