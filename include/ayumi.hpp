#ifndef LOD_AYUMI_HPP_
#define LOD_AYUMI_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/mesh/animatedMesh.hpp"
#include "oglwrap/utils/camera.hpp"

#include "charmove.hpp"
#include "skybox.hpp"
#include "shadow.hpp"

extern const float GRAVITY;
/* 0 -> max quality
   4 -> max performance */
extern const int PERFORMANCE;

class Ayumi {
  oglwrap::AnimatedMesh mesh_;
  oglwrap::Program prog_, shadow_prog_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_, uBones_, uShadowCP_;
  oglwrap::LazyUniform<glm::mat4> shadow_uMCP_, shadow_uBones_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<int> uNumUsedShadowMaps_, uShadowSoftness_;

  bool attack2_, attack3_;

  CharacterMovement& charmove_;

  struct AnimationEndedListener : public oglwrap::AnimatedMesh::AnimationEndedListener {
    Ayumi& ayumi;

    AnimationEndedListener(Ayumi& ayumi) : ayumi(ayumi) { }

    std::string operator()(const std::string& current_anim, 
                           float *transition_time,
                           bool *use_default_flags, 
                           unsigned *flags,
                           float *speed) override {

      *use_default_flags = true;

      if(current_anim == "Attack" && ayumi.attack2_) {
        *transition_time = 0.1f;
        return "Attack2";
      } else if(current_anim == "Attack2") {
        ayumi.attack2_ = false;
        if(ayumi.attack3_) {
          *transition_time = 0.05f;
          return "Attack3";
        }
      } else if(current_anim == "Attack3") {
        *transition_time = 0.4f;
        ayumi.attack3_ = false;
      }

      if(current_anim == "Flip") {
        *transition_time = 0.2f;
        ayumi.charmove_.setFlip(false);
        return "JumpFall";
      }

      if(ayumi.charmove_.isJumping()) {
        *transition_time = 0.3f;
        if(ayumi.charmove_.isJumpingRise()) {
          return "JumpRise";
        } else {
          return "JumpFall";
        }
      } else if(ayumi.charmove_.isWalking()) {
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
          if(current_anim == "Attack2") {
            *transition_time = 0.4f;
          } else {
            *transition_time = 0.3f;
          }
          return "Run";
        } else {
          if(current_anim == "Attack2") {
            *transition_time = 0.4f;
          } else {
            *transition_time = 0.2f;
          }
          return "Walk";
        }
      } else {
        if(current_anim == "Attack2") {
          *transition_time = 0.4f;
        } else {
          *transition_time = 0.2f;
        }
        return "Stand";
      }
    }
  } anim_end_listener_;

  struct CanJumpFunctor : public CharacterMovement::CanJumpFunctor {
    const Ayumi& ayumi;
    CanJumpFunctor(const Ayumi& ayumi) : ayumi(ayumi) { }
    bool operator()() const {
      return ayumi.canJump();
    }
  } can_jump_functor_;

  struct CanFlipFunctor : public CharacterMovement::CanFlipFunctor {
    const Ayumi& ayumi;
    CanFlipFunctor(const Ayumi& ayumi) : ayumi(ayumi) { }
    bool operator()() const {
      return ayumi.canFlip();
    }
  } can_flip_functor_;

  Skybox& skybox_;

public:
  Ayumi(Skybox& skybox, CharacterMovement& charmove);
  oglwrap::AnimatedMesh& getMesh();
  void resize(glm::mat4 projMat);
  void updateStatus(float time, const CharacterMovement& charmove);
  void shadowRender(float time, Shadow& shadow, const CharacterMovement& charmove);
  void render(float time, const oglwrap::Camera& cam,
              const CharacterMovement& charmove, const Shadow& shadow);
  bool canJump() const;
  bool canFlip() const;
};

#endif // LOD_AYUMI_HPP_
