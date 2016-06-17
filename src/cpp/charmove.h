// Copyright (c) 2014, Tamas Csala

#ifndef LOD_CHARMOVE_H_
#define LOD_CHARMOVE_H_

#include "engine/oglwrap_config.h"
#include "engine/scene.h"
#include "engine/camera.h"
#include "engine/transform.h"
#include "engine/game_object.h"
#include "engine/mesh/animation.h"

#include "./terrain.h"

class CharacterMovement : public engine::GameObject {
 public:
  CharacterMovement(engine::GameObject *parent,
                    float horizontal_speed = 10.0f,
                    float rotationSpeed_PerSec = M_PI);

  using CanDoCallback = bool();
  void setCanJumpCallback(std::function<bool()> can_jump_functor) {
    can_jump_functor_ = can_jump_functor;
  }
  void setCanFlipCallback(std::function<bool()> can_flip_functor) {
    can_flip_functor_ = can_flip_functor;
  }

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

 private:
  engine::Transform& transform_;

  // Current and destination rotation angles.
  double curr_rot_, dest_rot_;

  // Moving speed per second in OpenGL units.
  float rot_speed_, vert_speed_, horiz_speed_, horiz_speed_factor_;

  bool walking_, jumping_, flip_, can_flip_, transition_;

  engine::Animation *anim_;
  engine::Camera *camera_;

  std::function<bool()> can_jump_functor_;
  std::function<bool()> can_flip_functor_;

  virtual void update() override;
  virtual void keyAction(int key, int scancode, int action, int mods) override;
};

#endif  // LOD_CHARMOVE_H_
