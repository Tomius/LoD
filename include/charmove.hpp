/** @file charmove.hpp
    @brief Implements features related to character movement.
*/

#ifndef LOD_CHARMOVE_HPP_
#define LOD_CHARMOVE_HPP_

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

  bool walking_, jumping_, transition_;

public:
  CharacterMovement(glm::vec3 pos = glm::vec3(),
                    float horizontal_speed = 10.0f,
                    float rotationSpeed_PerSec = 180.0f);
  void update(const oglwrap::Camera& cam, glm::vec2 character_offset);
  void updateHeight(float groundHeight);
  bool is_jumping() const;
  bool is_jumping_rise() const;
  bool is_jumping_fall() const;
  bool isWalking() const;
  glm::mat4 getModelMatrix() const;
  glm::vec3 getPos() const;
};

#endif // LOD_CHARMOVE_HPP_
