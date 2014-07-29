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
  /**
   * @brief Loads in the mesh and the skeleton for an asset, and prepares it
   *        for animation.
   *
   * @param filename   The name of the file.
   * @param flags      The assimp post-process flags to use while loading the mesh.
   */
  AnimatedMeshRenderer(const std::string& filename,
                       gl::Bitfield<aiPostProcessSteps> flags);

  /// Returns a reference to the animation resources
  const AnimData& getAnimData() const { return anims_; }

  // ---------------------------- Skin definition ------------------------------

  /**
   * @brief Marks a bone to be modified from outside.
   *
   * @param bone_name   The name of the bone.
   * @return A structure, which through the bone, and all of its child can
   *         be moved.
   */
  ExternalBoneTree markBoneExternal(const std::string& bone_name);

  /**
   * @brief Returns the number of bones this scene has.
   *
   * May change the currently active VAO and ArrayBuffer at the first call.
   */
  size_t getNumBones();

  /**
   * @brief Returns the size that boneIds and BoneWeights attribute arrays
   *        should be.
   *
   * May change the currently active VAO and ArrayBuffer at the first call.
   */
  size_t getBoneAttribNum();

  /**
   * @brief Loads in bone weight and id information to the given array of
   *        attribute arrays.
   *
   * Uploads the bone weight and id to an array of attribute arrays, and sets
   * it up for use. For example if you specified "in vec4 boneIds[3]" you have
   * to give "prog | boneIds".
   *
   * Calling this function changes the currently active VAO and ArrayBuffer.
   *
   * @param boneIDs        The array of attributes array to use as destination
   *                       for bone IDs.
   * @param bone_weights   The array of attributes array to use as destination
   *                       for bone weights.
   * @param integerIDs     If true, boneIDs are uploaded as integers
   *                       (#version 130+) else they are uploaded as floats */
  void setupBones(gl::LazyVertexAttrib boneIDs,
                  gl::LazyVertexAttrib bone_weights,
                  bool integerIDs = true);

  // -------------------------------- Animation --------------------------------

  /// Updates the bones' transformations.
  void updateBoneInfo(Animation& animation,
                      float time_in_seconds);

  /**
   * @brief Uploads the bones' transformations into the given uniform array.
   *
   * @param bones - The uniform naming the bones array. It should be indexable.
   */
  void uploadBoneInfo(gl::LazyUniform<glm::mat4>& bones);

  /**
   * @brief Updates the bones transformation and uploads them into the given
   *        uniforms.
   *
   * @param animation        The animation to update.
   * @param time_in_seconds  Expect a time value as a float, optimally since
   *                         the start of the program.
   * @param bones            The uniform naming the bones array. It should be
   *                         indexable.
   */
  void updateAndUploadBoneInfo(Animation& animation,
                               float time_in_seconds,
                               gl::LazyUniform<glm::mat4>& bones);

  // --------------------------- Animation Control -----------------------------

  /**
   * @brief Adds an external animation from a file.
   *
   * You should give this animation a name, you will be able to
   * reference it with this name in the future. You can also set
   * the default animation modifier flags for this animation.
   * These flags will be used everytime you change to this animation
   * without explicitly specifying new flags.
   *
   * @param filename    The name of the file, from where to load the animation.
   * @param anim_name   The name with you wanna reference this animation.
   * @param flags       You can specify animation modifiers, like repeat the
   *                    animation after it ends, play it backwards, etc...
   * @param speed       Sets the default speed of the animation. If it's 1, it
   *                    will be played with the its default speed. If it's
   *                    negative, it will be played backwards.
   */
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

  /**
   * @brief A recursive functions that should be started from the root node, and
   * it returns the first bone under it.
   *
   * @param node   The current root node.
   * @param anim   The animation to seek the root bone in.
   */
  const aiNodeAnim* getRootBone(const aiNode* node, const aiScene* anim);

  template <typename Index_t>
  /**
   * @brief Creates bone attributes data.
   *
   * It is a template, as the type of boneIDs shouldn't be fix. Most of the times,
   * a skeleton won't contain more than 256 bones, but that doesn't mean boneIDs
   * should be forced to GLubyte, it works with shorts and even ints too. Although
   * I really doubt anyone would be using a skeleton with more than 65535 bones...
   */
  void loadBones();

  /**
   * @brief Creates the bone attributes data (the skinning.)
   *
   * It actually just calls the loadBones function with the appropriate template
   * parameter.
   */
  void createBonesData();

  template <typename Index_t>
  /**
   * Shader plumbs the bone data.
   *
   * It is a template, as the type of boneIDs shouldn't be fix. Most of the times,
   * a skeleton won't contain more than 256 bones, but that doesn't mean boneIDs
   * should be forced to GLubyte, it works with shorts and even ints too. Although
   * I really doubt anyone would be using a skeleton with more than 65535 bones...
   *
   * @param idx_t          The oglwrap enum, naming the data type that should be
   *                       used.
   * @param boneIDs        Should be an array of attributes, that will be shader
   *                       plumbed for the boneIDs data.
   * @param bone_weights   Should be an array of attributes, that will be shader
   *                       plumbed for the bone_weights data.
   * @param integerIDs     If true, boneIDs are uploaded as integers
   *                       (#version 130+) else they are uploaded as floats */
  void shaderPlumbBones(gl::IndexType idx_t,
                        gl::LazyVertexAttrib boneIDs,
                        gl::LazyVertexAttrib bone_weights,
                        bool integerWeights = true);

  /**
   * @brief Returns the first node called \a name, who is under \a currentRoot
   * in the bone hierarchy.
   *
   * Note: this function is recursive
   *
   * @param currentRoot   The bone under which to search.
   * @param name          The name of the bone that is to be found.
   * @return the handle to the bone that is called name, or nullptr.
   */
  aiNode* findNode(aiNode* currentRoot, const std::string& name);

  /**
   * @brief Marks all of a bone's child external recursively.
   *
   * @param parent              A pointer to the parent ExternalBone struct.
   * @param node                The current node.
   * @param should_be_external  Should be false if called from outside,
   *                            true if called recursively.
   */
  ExternalBone markChildExternal(ExternalBone* parent, aiNode* node,
                                 bool should_be_external = false);


  // -------------------------------- Animation --------------------------------

  /**
   * @brief Returns the index of the currently active translation keyframe for
   *        the given animation and time.
   *
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for a keyframe.
   */
  unsigned findPosition(float anim_time, const aiNodeAnim* node_anim);

  /**
   * @brief Returns the index of the currently active rotation keyframe for
   *        the given animation and time.
   *
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for a keyframe.
   */
  unsigned findRotation(float anim_time, const aiNodeAnim* node_anim);

  /**
   * @brief Returns the index of the currently active scaling keyframe for
   *        the given animation and time.
   *
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for a keyframe.
   */
  unsigned findScaling(float anim_time, const aiNodeAnim* node_anim);

  /**
   * @brief Returns a linearly interpolated value between the previous and next
   *        translation keyframes.
   *
   * @param out         Returns the result here.
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for the keyframes.
   */
  void calcInterpolatedPosition(aiVector3D& out, float anim_time,
                                const aiNodeAnim* node_anim);

  /**
   * @brief Returns a spherically interpolated value (always choosing the shorter
   * path) between the previous and next rotation keyframes.
   *
   * @param out         Returns the result here.
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for the keyframes.
   */
  void calcInterpolatedRotation(aiQuaternion& out, float anim_time,
                                const aiNodeAnim* node_anim);

  /**
   * @brief Returns a linearly interpolated value between the previous and next
   *        scaling keyframes.
   *
   * @param out         Returns the result here.
   * @param anim_time   The time elapsed since the start of this animation.
   * @param node_anim   The animation node, in which the function should search
   *                    for the keyframes.
   */
  void calcInterpolatedScaling(aiVector3D& out, float anim_time,
                               const aiNodeAnim* node_anim);

  /**
   * @brief Returns the animation node in the given animation, referenced by
   *        its name.
   *
   * Returns nullptr if it doesn't find a node with that name,
   * which usually means that it's not a bone.
   *
   * @param animation - The animation, this function should search in.
   * @param node_name - The name of the bone to search.
   */
  const aiNodeAnim* findNodeAnim(const aiAnimation* animation,
                                 const std::string node_name);

  /**
   * @brief Recursive function that travels through the entire node hierarchy,
   *        and creates transformation values in world space.
   *
   * Bone transformations are stored relative to their parents. That's why it is
   * needed. Also note, that the translation of the root node on the XZ plane is
   * treated differently, that offset isn't baked into the animation, you can get
   * the offset with the offsetSinceLastFrame() function, and you have to
   * externally do the object's movement, as normally it will stay right where
   * it was at the start of the animation.
   *
   * @param animation          The animation to update.
   * @param anim_time          The current animation time.
   * @param node               The node (bone) whose, and whose child's
   *                           transformation should be updated. You should call
   *                           this function with the root node.
   * @param parent_transform   The transformation of the parent node. You should
   *                           call it with an identity matrix.
   */
  void updateBoneTree(Animation& animation,
                      float anim_time,
                      const aiNode* node,
                      const glm::mat4& parent_transform = glm::mat4());

  /**
   * @brief Does the same thing as readNodeHierarchy, but it is used to create
   *        transitions between animations, so it interpolates between four
   *        keyframes not two.
   *
   * @param animation             The animation to update.
   * @param prev_animation_time   The animation time of when, the last animation
   *                              was interrupted.
   * @param next_animation_time   The current animation time.
   * @param node                  The node (bone) whose, and whose child's
   *                              transformation should be updated. You should
   *                              call this function with the root node.
   * @param parent_transform      The transformation of the parent node. You
   *                              should call it with an identity matrix.
   */
  void updateBoneTreeInTransition(Animation& animation,
                                  float prev_animation_time,
                                  float next_animation_time,
                                  float factor,
                                  const aiNode* node,
                                  const glm::mat4& parent_transform = glm::mat4());

};  // AnimatedMeshRenderer
}  // namespace engine

#endif  // ENGINE_MESH_ANIMATED_MESH_RENDERER_H_
