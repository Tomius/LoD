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
  /**
   * @brief A callback function, that is called everytime an animation ends
   *        The function should return the name of the new animation. If it
   *        returns a string that isn't a name of an animation, then the default
   *        animation will be played.
   *
   * It is typically created using std::bind, like this:
   *
   * using std::placeholders::_1;
   * auto callback = std::bind(&MyClass::animationEndedCallback, this, _1);
   *
   * Or, if you prefer you can use lambdas, to create the callback:
   *
   * auto callback2 = [this](const std::string& current_anim){
   *   return animationEndedCallback(current_anim);
   *  };
   *
   * @param current_anim  The name of the currently playing animation, that is
   *                      about to be changed.
   * @return The parameters of the new animation.
   */
  using AnimationEndedCallback =
    AnimParams(const std::string& current_anim);

private:
  /// The callback functor
  std::function<AnimationEndedCallback> anim_ended_callback_;

  /**
   * @brief The function that changes animations when they end.
   *
   * @param current_time  The current time.
   */
  void animationEnded(float current_time);

public:
  /**
   * @brief Sets the callback functor for choosing the next anim.
   *
   * Sets a callback functor that is called everytime an animation ends,
   * and is responsible for choosing the next animation.
   *
   * @param callback - The functor to use for the callbacks.
   */
  void setAnimationEndedCallback(std::function<AnimationEndedCallback> callback) {
    anim_ended_callback_ = callback;
  }

  /**
   * @brief Sets the default animation, that will be played if you don't set to
   *        play another one.
   *
   * @param anim_name                 The user-defined name of the animation
   *                                  that should be set to be default.
   * @param default_transition_time   The fading time that should be used when
   *                                  changing to the default animation.
   */
  void setDefaultAnimation(const std::string& anim_name,
                           float default_transition_time = 0.0f);

private:
  /**
   * @brief Changes the current animation to a specified one.
   *
   * @param anim_idx          The index of the new animation.
   * @param current_time      The current time in seconds, optimally since the
   *                          start of the program.
   * @param transition_time   The fading time to be used for the transition.
   * @param flags             A bitfield containing the animation specifier flags.
   * @param speed             Sets the speed of the animation. If it's 0, will
   *                          play with the speed specified at the addAnim. If
   *                          it's negative, it will be played backwards.
   */
  void changeAnimation(size_t anim_idx,
                       float current_time,
                       float transition_time,
                       gl::Bitfield<AnimFlag> flags,
                       float speed = 1.0f);

public:
  /**
   * @brief Tries to change the current animation to a specified one.
   *
   * Only changes it if the current animation is interruptable,
   * it's not currently in a transition, and new animation is
   * not the same as the one currently playing.
   *
   * @param new_anim       The parameters of the new animation.
   * @param current_time   The current time in seconds, optimally since the
   *                       start of the program.
   */
  void setCurrentAnimation(AnimParams new_anim,
                           float current_time);

  /**
   * @brief Forces the current animation to a specified one.
   *
   * Only changes it if the new animation is not the same as the one
   * currently playing.
   *
   * @param new_anim       The parameters of the new animation.
   * @param current_time   The current time in seconds, optimally since the
   *                       start of the program.
   */
  void forceCurrentAnimation(AnimParams new_anim,
                             float current_time);

  /**
   * Tries to change the current animation to the default one.
   *
   * Only changes it if the current animation is interruptable,
   * it's not currently in a transition, and new animation is
   * not the same as the one currently playing. Will use the default
   * anim modifier flags for the default anim.
   *
   * @param current_time  The current time in seconds, optimally since the
   *                      start of the program.
   */
  void setAnimToDefault(float current_time);

  /**
   * @brief Forces the current animation to the default one.
   *
   * Only changes it if the new animation is not the same as the one currently
   * playing. Will use the default anim modifier flags for the default anim.
   *
   * @param current_time    The current time in seconds, optimally since the
   *                        start of the program.
   */
  void forceAnimToDefault(float current_time);

  /**
   * @brief Returns the offset of the root bone, since it was last queried.
   *
   * It should be queried every frame (hence the name),
   * but it works even if you only query every 10th frame,
   * just the animation will "lag", and will look bad.
   */
  glm::vec2 offsetSinceLastFrame();
};

} // namespace engine

#endif
