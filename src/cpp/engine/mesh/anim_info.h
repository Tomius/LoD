// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MESH_ANIM_INFO_H_
#define ENGINE_MESH_ANIM_INFO_H_

#include <map>
#include <memory>

#include "../oglwrap_config.h"

#include "anim_state.h"
#include "../assimp.h"

namespace engine {

/// A struct storing info per animation
struct AnimInfo {
  /// The importer that stores the animations.
  // It is a shared_ptr because we want to use AnimInfo
  // in std::vector, which needs copy ctor
  std::shared_ptr<Assimp::Importer> importer;

  /// Handle for the animations
  const aiScene* handle;

  /// The name of the animation.
  std::string name;

  /// Default animation flags for this animation.
  gl::Bitfield<AnimFlag> flags;

  /// Default speed modifier.
  float speed;

  /// The offset value at the starts of the animation.
  glm::vec3 start_offset;

  /// The offset values at the ends of the animations.
  glm::vec3 end_offset;

  /// Default constructor
  AnimInfo()
      : importer(new Assimp::Importer{})
      , handle(nullptr)
      , flags(0)
      , speed(1.0f)
  { }
};

struct AnimMetaInfo {
  /// The index of the default animation.
  size_t default_idx;

  /// The fading time that is used when changing the animation back to the default one.
  float default_transition_time;

  /// The fading time between the previous and the current animation.
  float transition_time;

  /// The time of when did the last animation end.
  /** It is needed to know the time in the current animation. */
  float end_of_last_anim;

  /// The animation of time the previous animation.
  /** It is needed to make the transition between two animations. */
  float last_period_time;

  /// It is used to detect when did the animation start a new cycle.
  /** For animations that have AnimFlag::Repeat flag specified only, of course. */
  unsigned last_loop_count;

  AnimMetaInfo()
      : default_idx(0)
      , default_transition_time(0.0f)
      , transition_time(0.0f)
      , end_of_last_anim(0.0f)
      , last_period_time(0.0f)
      , last_loop_count(0.0f)
  { }
};

struct AnimData {
  /// Handles for the animations
  std::vector<AnimInfo> data;

  /// Maps user defined animation names to indices.
  std::map<std::string, size_t> names;

  AnimInfo& operator[](size_t idx) {
    return data[idx];
  }

  const AnimInfo& operator[](size_t idx) const {
    return data[idx];
  }

  AnimInfo& operator[](const std::string& anim_name) {
    return data[names.at(anim_name)];
  }

  const AnimInfo& operator[](const std::string& anim_name) const {
    return data[names.at(anim_name)];
  }

  bool canFind(const std::string& anim_name) const {
    return names.find(anim_name) != names.end();
  }
};

} // namespace engine

#endif  // ENGINE_MESH_ANIM_INFO_H_
