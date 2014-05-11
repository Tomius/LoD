/** @file charmove.h
    @brief Implements features related to character movement.
*/

#ifndef LOD_CHARMOVE_H_
#define LOD_CHARMOVE_H_

#include <exception>

#include "oglwrap_config.h"
#include "engine/camera.h"
#include "engine/transform.h"
#include "engine/mesh/animated_mesh_renderer.h"

#include "terrain.h"

extern const float GRAVITY;

class CharacterMovement {
  engine::Transform& transform_;
  engine::RigidBody& rigid_body_;

  // Current and destination rotation angles.
  double curr_rot_, dest_rot_;

  // Moving speed per second in OpenGL units.
  float rot_speed_, vert_speed_, horiz_speed_, horiz_speed_factor_;

  bool walking_, jumping_, flip_, can_flip_, transition_;

  engine::Animation *anim_;
  engine::Camera *camera_;

public:
  using CanDoCallback = bool();

private:
  std::function<CanDoCallback> can_jump_functor_;
  std::function<CanDoCallback> can_flip_functor_;

public:
  CharacterMovement(engine::Transform& transform,
                    engine::RigidBody& rigid_body,
                    float horizontal_speed = 10.0f,
                    float rotationSpeed_PerSec = 180.0f);

  void setCanJumpCallback(std::function<CanDoCallback> can_jump_functor) {
    can_jump_functor_ = can_jump_functor;
  }
  void setCanFlipCallback(std::function<CanDoCallback> can_flip_functor) {
    can_flip_functor_ = can_flip_functor;
  }
  void update(float time);
  void handleSpacePressed();
  void updateHeight(float time);
  bool isJumping() const;
  bool isJumpingRise() const;
  bool isJumpingFall() const;
  bool isDoingFlip() const;
  void setFlip(bool flip);
  bool isWalking() const;
  void setAnimation(engine::Animation* anim) { anim_ = anim; }
  void setCamera(engine::Camera* cam) { camera_ = cam; }
};

#endif // LOD_CHARMOVE_H_
