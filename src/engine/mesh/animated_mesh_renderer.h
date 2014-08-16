// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MESH_ANIMATED_MESH_RENDERER_H_
#define ENGINE_MESH_ANIMATED_MESH_RENDERER_H_

#include <string>
#include <functional>

#include "../oglwrap_config.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/smart_enums.h"

#include "./mesh_renderer.h"
#include "./anim_state.h"
#include "./skinning_data.h"
#include "./anim_info.h"

namespace engine {

class Animation;

/// A class for loading and displaying animations.
class AnimatedMeshRenderer : public MeshRenderer {
  /// Stores data related to skin definition.
  SkinningData skinning_data_;

  /// The animations.
  AnimData anims_;

 public:
  AnimatedMeshRenderer(const std::string& filename,
                       gl::Bitfield<aiPostProcessSteps> flags);

  /// Returns a reference to the animation resources
  const AnimData& getAnimData() const { return anims_; }

  // ---------------------------- Skin definition ------------------------------

  ExternalBoneTree markBoneExternal(const std::string& bone_name);

  size_t getNumBones();

  size_t getBoneAttribNum();

  void setupBones(gl::LazyVertexAttrib boneIDs,
                  gl::LazyVertexAttrib bone_weights,
                  bool integerIDs = true);

  // -------------------------------- Animation --------------------------------

  /// Updates the bones' transformations.
  void updateBoneInfo(Animation& animation,
                      float time_in_seconds);

  void uploadBoneInfo(gl::LazyUniform<glm::mat4>& bones);

  void updateAndUploadBoneInfo(Animation& animation,
                               float time_in_seconds,
                               gl::LazyUniform<glm::mat4>& bones);

  // --------------------------- Animation Control -----------------------------

  void addAnimation(const std::string& filename,
                    const std::string& anim_name,
                    gl::Bitfield<AnimFlag> flags = AnimFlag::None,
                    float speed = 1.0f);

 private:
  /// It shouldn't be copyable.
  AnimatedMeshRenderer(const AnimatedMeshRenderer& src) = delete;

  /// It shouldn't be copyable.
  void operator=(const AnimatedMeshRenderer& rhs) = delete;

  // ---------------------------- Skin definition ------------------------------

  /// Fills the bone_mapping with data.
  void mapBones();

  const aiNodeAnim* getRootBone(const aiNode* node, const aiScene* anim);

  template <typename Index_t>
  void loadBones();

  void createBonesData();

  template <typename Index_t>
  void shaderPlumbBones(gl::IndexType idx_t,
                        gl::LazyVertexAttrib boneIDs,
                        gl::LazyVertexAttrib bone_weights,
                        bool integerWeights = true);

  aiNode* findNode(aiNode* currentRoot, const std::string& name);

  ExternalBone markChildExternal(ExternalBone* parent, aiNode* node,
                                 bool should_be_external = false);


  // -------------------------------- Animation --------------------------------

  unsigned findPosition(float anim_time, const aiNodeAnim* node_anim);

  unsigned findRotation(float anim_time, const aiNodeAnim* node_anim);

  unsigned findScaling(float anim_time, const aiNodeAnim* node_anim);

  void calcInterpolatedPosition(aiVector3D& out, float anim_time,
                                const aiNodeAnim* node_anim);

  void calcInterpolatedRotation(aiQuaternion& out, float anim_time,
                                const aiNodeAnim* node_anim);

  void calcInterpolatedScaling(aiVector3D& out, float anim_time,
                               const aiNodeAnim* node_anim);

  const aiNodeAnim* findNodeAnim(const aiAnimation* animation,
                                 const std::string node_name);

  void updateBoneTree(Animation& animation,
                      float anim_time,
                      const aiNode* node,
                      const glm::mat4& parent_transform = glm::mat4());

  void updateBoneTreeInTransition(Animation& animation,
                                  float prev_animation_time,
                                  float next_animation_time,
                                  float factor,
                                  const aiNode* node,
                                  const glm::mat4& parent_transform = glm::mat4());

};  // AnimatedMeshRenderer
}  // namespace engine

#endif  // ENGINE_MESH_ANIMATED_MESH_RENDERER_H_
