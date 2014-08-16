// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MESH_ANIMATATION_H_
#define ENGINE_MESH_ANIMATATION_H_

#include "anim_info.h"
#include "animated_mesh_renderer.h"

namespace engine {

class Animation {
  /// Reference to the animation resources held by the AnimatedMeshRenderer.
  const AnimData& anims_;

  /// Stores data to handle animation transitions.
  AnimMetaInfo anim_meta_info_;

  /// The current animation.
  AnimationState current_anim_;

  /// The name of the current animation
  std::string current_anim_name_;

  /// The last animation.
  AnimationState last_anim_;

  friend class AnimatedMeshRenderer;

public:

  Animation(const AnimData& anim_data)
    : anims_(anim_data) {}

  /// Returns the currently running animation's name.
  std::string getCurrentAnimation() const {
    return current_anim_name_;
  }

  /// Returns the currently running animation's state.
  AnimationState getCurrentAnimState() const {
    return current_anim_;
  }

  /// Returns the currently running animation's AnimFlags.
  gl::Bitfield<AnimFlag> getCurrentAnimFlags() const {
    return current_anim_.flags;
  }

  /// Returns if the currently running animation is interruptable.
  bool isInterrupable() const {
    return current_anim_.flags.test(AnimFlag::Interruptable);
  }

  /// Returns the name of the default animation
  std::string getDefaultAnim() const {
    return anims_[anim_meta_info_.default_idx].name;
  }

public:
  using AnimationEndedCallback =
    AnimParams(const std::string& current_anim);

private:
  /// The callback functor
  std::function<AnimationEndedCallback> anim_ended_callback_;

  void animationEnded(float current_time);

public:
  void setAnimationEndedCallback(std::function<AnimationEndedCallback> callback) {
    anim_ended_callback_ = callback;
  }

  void setDefaultAnimation(const std::string& anim_name,
                           float default_transition_time = 0.0f);

private:
  void changeAnimation(size_t anim_idx,
                       float current_time,
                       float transition_time,
                       gl::Bitfield<AnimFlag> flags,
                       float speed = 1.0f);

public:
  void setCurrentAnimation(AnimParams new_anim,
                           float current_time);

  void forceCurrentAnimation(AnimParams new_anim,
                             float current_time);

  void setAnimToDefault(float current_time);

  void forceAnimToDefault(float current_time);

  glm::vec2 offsetSinceLastFrame();
};

} // namespace engine

#endif
