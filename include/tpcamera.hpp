#ifndef LOD_TPCAMERA_HPP_
#define LOD_TPCAMERA_HPP_

#include <SFML/Window.hpp>

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

#include "../engine/camera.hpp"

#include "charmove.hpp"

class TPCamera : public engine::ThirdPersonalCamera {
  sf::Window& window_;
  const bool& fixMouse_;
  CharacterMovement& charmove_;
  glm::vec3 offset_;

public:
  TPCamera(sf::Window& window,
           const bool& fixMouse,
           CharacterMovement& charmove,
           glm::vec3 offset,
           const glm::vec3& pos,
           const glm::vec3& target = glm::vec3(),
           float mouse_sensitivity = 1.0f,
           float mouse_scroll_sensitivity = 1.0f)
    : ThirdPersonalCamera(pos, target, mouse_sensitivity, mouse_scroll_sensitivity)
    , window_(window)
    , fixMouse_(fixMouse)
    , charmove_(charmove)
    , offset_(offset)
  { }

  void update(float time) {
    updateRotation(window_, fixMouse_);
    updateTarget(charmove_.getPos() + offset_);
  }
};


#endif
