/** @file charmove.hpp
    @brief Implements features related to character movement.
*/

#ifndef LOD_CHARMOVE_HPP_
#define LOD_CHARMOVE_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/utils/camera.hpp"

extern const float GRAVITY;

class CharacterMovement {
  glm::vec3 pos_;

  // Current and destination rotation angles.
  double curr_rot_, dest_rot_;

  // Moving speed per second in OpenGL units.
  float rot_speed_, vert_speed_, horiz_speed_;

  bool walking_, jumping_, flip_, can_flip_, transition_;

public:
  struct CanJumpFunctor {
    virtual bool operator()() const = 0;
  };

  struct CanFlipFunctor {
    virtual bool operator()() const = 0;
  };

private:

  CanJumpFunctor* can_jump_functor_;
  CanFlipFunctor* can_flip_functor_;

public:
  CharacterMovement(glm::vec3 pos = glm::vec3(),
                    float horizontal_speed = 10.0f,
                    float rotationSpeed_PerSec = 180.0f);
  
  void setCanJumpFunctor(CanJumpFunctor *can_jump_functor) { 
    can_jump_functor_ = can_jump_functor; 
  }
  void setCanFlipFunctor(CanFlipFunctor *can_flip_functor) { 
    can_flip_functor_ = can_flip_functor; 
  }
  void update(const oglwrap::Camera& cam, glm::vec2 character_offset);
  void handleSpacePressed();
  void updateHeight(float groundHeight);
  bool isJumping() const;
  bool isJumpingRise() const;
  bool isJumpingFall() const;
  bool isDoingFlip() const;
  void setFlip(bool flip);
  bool isWalking() const;
  glm::mat4 getModelMatrix() const;
  glm::vec3 getPos() const;
};

#endif // LOD_CHARMOVE_HPP_
