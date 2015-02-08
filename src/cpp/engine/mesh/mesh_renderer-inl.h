// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MESH_MESH_RENDERER_INL_H_
#define ENGINE_MESH_MESH_RENDERER_INL_H_

#include <vector>
#include "./mesh_renderer.h"

namespace engine {

template <typename IdxType>
/// Returns a vector of the indices
std::vector<IdxType> MeshRenderer::indices() {
  std::vector<IdxType> indices_vector;

  for (int mesh_idx = 0; mesh_idx < scene_->mNumMeshes; ++mesh_idx) {
    const aiMesh* mesh = scene_->mMeshes[mesh_idx];
    indices_vector.reserve(indices_vector.size() + mesh->mNumFaces * 3);
    for (size_t face_idx = 0; face_idx < mesh->mNumFaces; face_idx++) {
      const aiFace& face = mesh->mFaces[face_idx];
      if (face.mNumIndices == 3) {  // The invalid faces are just ignored.
        indices_vector.push_back(face.mIndices[0]);
        indices_vector.push_back(face.mIndices[1]);
        indices_vector.push_back(face.mIndices[2]);
      }
    }
  }

  return indices_vector;
}

}  // namespace engine

#endif
