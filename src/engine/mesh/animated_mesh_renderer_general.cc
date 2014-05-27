// Copyright (c) 2014, Tamas Csala

#include "animated_mesh_renderer.h"

namespace engine {

AnimatedMeshRenderer::AnimatedMeshRenderer(
                                  const std::string& filename,
                                  gl::Bitfield<aiPostProcessSteps> flags)
  : MeshRenderer(filename, flags)
  , skinning_data_(scene_->mNumMeshes) {
}

void AnimatedMeshRenderer::addAnimation(const std::string& filename,
                                        const std::string& anim_name,
                                        gl::Bitfield<AnimFlag> flags,
                                        float speed) {
  if (anims_.canFind(anim_name)) {
    throw std::runtime_error(
      "Animation name '" + anim_name + "' isn't unique for '" + filename + "'"
    );
  }
  size_t idx = anims_.data.size();
  anims_.names[anim_name] = idx;
  anims_.data.push_back(AnimInfo());
  anims_[idx].name = anim_name;
  anims_[idx].handle = anims_[idx].importer->ReadFile(filename, aiProcess_Debone);
  if (!anims_[idx].handle) {
    throw std::runtime_error("Error parsing " + filename
                              + " : " + anims_[idx].importer->GetErrorString());
  }

  auto node = getRootBone(scene_->mRootNode, anims_[idx].handle);
  if (!node) {
    throw std::runtime_error(
      "Animation error: The mesh's skeleton, and the animated skeleton '"
      + anim_name + "' doesn't have a single bone in common."
    );
  }

  aiVector3D v = node->mPositionKeys[0].mValue;
  anims_[idx].start_offset = glm::vec3(v.x, v.y, v.z);

  v = node->mPositionKeys[node->mNumPositionKeys - 1].mValue;
  anims_[idx].end_offset =  glm::vec3(v.x, v.y, v.z);

  anims_[idx].flags = flags;
  anims_[idx].speed = speed;
}

} // namespace engine
