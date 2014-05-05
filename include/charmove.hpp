/** @file charmove.hpp
    @brief Implements features related to character movement.
*/

#ifndef LOD_CHARMOVE_HPP_
#define LOD_CHARMOVE_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/utils/camera.hpp"
#include "../engine/mesh/animated_mesh_renderer.hpp"
#include "terrain.hpp"

extern const float GRAVITY;

class CharacterMovement {
  glm::vec3 pos_;

  // Current and destination rotation angles.
  double curr_rot_, dest_rot_;

  // Moving speed per second in OpenGL units.
  float rot_speed_, vert_speed_, horiz_speed_, horiz_speed_factor_;

  bool walking_, jumping_, flip_, can_flip_, transition_;

  engine::AnimatedMeshRenderer *anim_mesh_;
  oglwrap::Camera *camera_;
  const Terrain& terrain_;

public:
  using CanDoCallback = bool();

private:

  std::function<CanDoCallback> can_jump_functor_;
  std::function<CanDoCallback> can_flip_functor_;

public:
  CharacterMovement(glm::vec3 pos,
                    const Terrain& terrain,
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
  void updateHeight(float time, float groundHeight);
  bool isJumping() const;
  bool isJumpingRise() const;
  bool isJumpingFall() const;
  bool isDoingFlip() const;
  void setFlip(bool flip);
  bool isWalking() const;
  glm::mat4 getModelMatrix() const;
  glm::vec3 getPos() const;
  void setAnimatedMesh(engine::AnimatedMeshRenderer* mesh) {
    anim_mesh_ = mesh;
  }

  void setCamera(oglwrap::Camera* cam) {
    camera_ = cam;
  }
};

#endif // LOD_CHARMOVE_HPP_
