/** @file charmove.hpp
    @brief Implements features related to character movement.
*/

#ifndef LOD_CHARMOVE_HPP_
#define LOD_CHARMOVE_HPP_

#include "oglwrap/utils/camera.hpp"

extern const float GRAVITY;

namespace oglwrap {

class CharacterMovement {
  glm::vec3 pos_;

  // Current and destination rotation angles.
  double currRot_, destRot_;

  // Moving speed per second in OpenGL units.
  float rotSpeed_, vertSpeed_, horizSpeed_;

  bool walking_, jumping_, transition_;

public:
  CharacterMovement(glm::vec3 pos = glm::vec3(),
                    float horizontal_speed = 10.0f,
                    float rotationSpeed_PerSec = 180.0f)
    : pos_(pos)
    , currRot_(0)
    , destRot_(0)
    , rotSpeed_(rotationSpeed_PerSec)
    , vertSpeed_(0)
    , horizSpeed_(horizontal_speed)
    , walking_(false)
    , jumping_(false)
    , transition_(false)
  { }

  void update(const Camera& cam, glm::vec2 character_offset) {
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

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !jumping_) {
      jumping_ = true;
      vertSpeed_ = 0.5f;
    }


    if(walking_) {
      double cameraRot = -cam.getRoty(); // -z is forward
      double moveRot = atan2(moveDir.y, moveDir.x); // +y is forward
      destRot_ = ToDegree(cameraRot + moveRot);
      destRot_ = fmod(destRot_, 360);

      double diff = destRot_ - currRot_;
      double sign = diff / fabs(diff);

      // Take the shorter path.
      while(fabs(diff) > 180) {
        destRot_ -= sign * 360;

        diff = destRot_ - currRot_;
        sign = diff / fabs(diff);
      }

      if(transition_) {
        if(fabs(diff) > rotSpeed_ / 20.0f) {
          currRot_ += sign * dt * rotSpeed_;
          currRot_ = fmod(currRot_, 360);
        } else {
          transition_ = false;
        }
      } else {
        currRot_ = fmod(destRot_, 360);
      }
    }

    mat3 transformation = mat3(rotate(mat4(), (float)fmod(currRot_, 360), vec3(0,1,0)));

    pos_ += transformation * vec3(character_offset.x, 0, character_offset.y);
    if(jumping_) {
      pos_ += transformation * vec3(0, 0, horizSpeed_ * dt);
    }
  }

  void updateHeight(float groundHeight) {
    static sf::Clock clock;
    static float prevTime = 0;
    float time = clock.getElapsedTime().asSeconds();
    float dt =  time - prevTime;
    prevTime = time;

    const float diff = groundHeight - pos_.y;
    if(diff >= 0 && jumping_ && vertSpeed_ < 0) {
      jumping_ = false;
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
        pos_.y += std::max(diff, vertSpeed_) * dt * 30.0f;
      } else {
        pos_.y += vertSpeed_ * dt * 30.0f;
      }
      vertSpeed_ -= dt * GRAVITY;
    }
  }

  bool is_jumping() const {
    return jumping_;
  }

  bool is_jumping_rise() const {
    return jumping_ && vertSpeed_ > 0;
  }

  bool is_jumping_fall() const {
    return jumping_ && vertSpeed_ < 0;
  }

  bool isWalking() const {
    return walking_;
  }

  glm::mat4 getModelMatrix() const {
    glm::mat4 rot = glm::rotate(glm::mat4(), (float)fmod(currRot_, 360), glm::vec3(0,1,0));
    // The matrix's last column is responsible for the translation.
    rot[3] = glm::vec4(pos_, 1.0);
    return rot;
  }

  glm::vec3 getPos() const {
    return pos_;
  }
};

}

#endif // LOD_CHARMOVE_HPP_
