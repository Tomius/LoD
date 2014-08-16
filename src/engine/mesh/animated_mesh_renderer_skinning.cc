// Copyright (c) 2014, Tamas Csala

#include <vector>
#include <limits>
#include <string>
#include "./animated_mesh_renderer.h"

namespace engine {

/// Fills the bone_mapping with data.
void AnimatedMeshRenderer::mapBones() {
  for (size_t entry = 0; entry < entries_.size(); entry++) {
    const aiMesh* mesh = scene_->mMeshes[entry];

    for (size_t i = 0; i < mesh->mNumBones; i++) {
      std::string bone_name(mesh->mBones[i]->mName.data);
      size_t bone_index = 0;

      // Search for this bone in the BoneMap
      if (skinning_data_.bone_mapping.find(bone_name)
            == skinning_data_.bone_mapping.end()) {
        // Allocate an index for the new bone
        bone_index = skinning_data_.num_bones++;
        skinning_data_.bone_info.push_back(SkinningData::BoneInfo());
        skinning_data_.bone_info[bone_index].bone_offset =
          engine::convertMatrix(mesh->mBones[i]->mOffsetMatrix);
        skinning_data_.bone_mapping[bone_name] = bone_index;
      }
    }
  }
}

const aiNodeAnim* AnimatedMeshRenderer::getRootBone(const aiNode* node,
                                                    const aiScene* anim) {
  std::string node_name(node->mName.data);

  const aiAnimation* animation = anim->mAnimations[0];
  const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

  if (node_anim) {
    if (skinning_data_.root_bone.empty()) {
      skinning_data_.root_bone = node_name;
    } else {
      if (skinning_data_.root_bone != node_name) {
        throw std::runtime_error("Animation error: the animated skeletons have "
                                 "different root bones.");
      }
    }
    return node_anim;
  } else {
    for (size_t i = 0; i < node->mNumChildren; i++) {
      auto childsReturn = getRootBone(node->mChildren[i], anim);
      if (childsReturn) {
        return childsReturn;
      }
    }
  }

  return nullptr;
}

template <typename Index_t>
/// Creates bone attributes data.
void AnimatedMeshRenderer::loadBones() {
  const size_t per_attrib_size =
      sizeof(SkinningData::VertexBoneData_PerAttribute<Index_t>);

  skinning_data_.per_mesh_attrib_max.resize(entries_.size());

  for (size_t entry = 0; entry < entries_.size(); entry++) {
    std::vector<SkinningData::VertexBoneData<Index_t>> vertices;
    const aiMesh* mesh = scene_->mMeshes[entry];
    vertices.resize(mesh->mNumVertices);

    // -------======{[ Create the bone ID's and weights data ]}======-------

    for (size_t i = 0; i < mesh->mNumBones; i++) {
      std::string bone_name(mesh->mBones[i]->mName.data);
      size_t bone_index = skinning_data_.bone_mapping[bone_name];

      for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
        size_t vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
        float weight = mesh->mBones[i]->mWeights[j].mWeight;
        vertices[vertexID].AddBoneData(bone_index, weight);
      }
    }

    // -------======{[ Upload the bone data ]}======-------

    gl::BoundVertexArray bound_vao{entries_[entry].vao};
    gl::BoundBuffer bound_buffer{skinning_data_.vertex_bone_data_buffers[entry]};

    // I can't just upload to the buffer with .data(), as bones aren't stored
    // in a continuous buffer, and it is an array of not fixed sized arrays,
    // but OpenGL needs it in fix sized parts.

    // Get the current number of max bone attributes.
    unsigned char& current_attrib_max =
        skinning_data_.per_mesh_attrib_max[entry];
    for (size_t i = 0; i < vertices.size(); i++) {
      if (vertices[i].data.size() > current_attrib_max) {
        current_attrib_max = vertices[i].data.size();
      }
    }

    if (current_attrib_max > skinning_data_.max_bone_attrib_num) {
      skinning_data_.max_bone_attrib_num = current_attrib_max;
    }

    size_t per_vertex_size = current_attrib_max * per_attrib_size;
    size_t buffer_size = vertices.size() * per_vertex_size;

    // Upload the bones data into a continuous
    // buffer then upload that to OpenGL.
    std::unique_ptr<GLbyte> data{new GLbyte[buffer_size]};
    GLintptr offset = 0;
    for (size_t i = 0; i < vertices.size(); i++) {
      size_t curr_size = vertices[i].data.size() * per_attrib_size;

      // Copy the bone data
      memcpy(data.get() + offset,  // destination
             vertices[i].data.data(),  // source
             curr_size);  // length

      // Zero out all the remaining memory. Remember a
      // bone with a 0.0f weight doesn't have any influence
      if (per_vertex_size > curr_size) {
        memset(data.get() + offset + curr_size,  // memory place
               0,  // value
               per_vertex_size - curr_size);  // length
      }

      offset += per_vertex_size;
    }

    // upload
    bound_buffer.data(buffer_size, data.get());
  }
}

void AnimatedMeshRenderer::createBonesData() {
  mapBones();

  if (skinning_data_.num_bones < std::numeric_limits<GLubyte>::max()) {
    loadBones<GLubyte>();
  } else if (skinning_data_.num_bones < std::numeric_limits<GLushort>::max()) {
    loadBones<GLushort>();
  } else {  // more than 65535 bones? WTF???
    loadBones<GLuint>();
  }
}

template <typename Index_t>
void AnimatedMeshRenderer::shaderPlumbBones(
    gl::IndexType idx_t,
    gl::LazyVertexAttrib boneIDs,
    gl::LazyVertexAttrib bone_weights,
    bool integerIDs) {
  const size_t per_attrib_size =
      sizeof(SkinningData::VertexBoneData_PerAttribute<Index_t>);

  for (size_t entry = 0; entry < entries_.size(); entry++) {
    gl::BoundVertexArray bound_vao{entries_[entry].vao};
    gl::BoundBuffer bound_buffer{skinning_data_.vertex_bone_data_buffers[entry]};
    unsigned char current_attrib_max = skinning_data_.per_mesh_attrib_max[entry];

    for (unsigned char boneAttribSet = 0;
         boneAttribSet < current_attrib_max;
         boneAttribSet++) {
      const size_t stride = current_attrib_max * per_attrib_size;

      intptr_t baseOffset = boneAttribSet * per_attrib_size;
      intptr_t weightOffset = baseOffset + 4 * sizeof(Index_t);

      if (integerIDs) {
        boneIDs[boneAttribSet].ipointer(4, gl::WholeDataType(idx_t), stride,
                                        (const void*)baseOffset).enable();
      } else {
        boneIDs[boneAttribSet].pointer(4, gl::DataType(idx_t), false, stride,
                                       (const void*)baseOffset).enable();
      }

      bone_weights[boneAttribSet].setup(4, gl::DataType::kFloat, stride,
                                        (const void*)weightOffset).enable();
    }

    // static setup the VertexArrays that aren't enabled, to all zero.
    // Remember (0, 0, 0, 1) is the default, which isn't what we want.
    for (int i = current_attrib_max;
         i < skinning_data_.max_bone_attrib_num;
         i++) {
      if (integerIDs) {
        boneIDs[i].static_setup(glm::ivec4(0, 0, 0, 0));
      } else {
        boneIDs[i].static_setup(glm::vec4(0, 0, 0, 0));
      }
      bone_weights[i].static_setup(glm::vec4(0, 0, 0, 0));
    }
  }
}

aiNode* AnimatedMeshRenderer::findNode(aiNode* currentRoot,
                                       const std::string& name) {
  if (currentRoot->mName.data == name)
    return currentRoot;

  for (size_t i = 0; i != currentRoot->mNumChildren; ++i) {
    aiNode* children_return = findNode(currentRoot->mChildren[i], name);
    if (children_return)
      return children_return;
  }

  return nullptr;
}

ExternalBone AnimatedMeshRenderer::markChildExternal(ExternalBone* parent,
                                                     aiNode* node,
                                                     bool should_be_external) {
  size_t bidx = skinning_data_.bone_mapping[node->mName.data];
  SkinningData::BoneInfo& binfo = skinning_data_.bone_info[bidx];
  binfo.external = should_be_external;
  ExternalBone ebone(node->mName.data,
                     binfo.bone_offset,
                     engine::convertMatrix(node->mTransformation),
                     binfo.final_transform,
                     parent);

  for (size_t i = 0; i < node->mNumChildren; ++i) {
    ebone.child.push_back(markChildExternal(&ebone, node->mChildren[i], true));
  }

  return ebone;
}

/// Marks a bone to be modified from outside.
ExternalBoneTree AnimatedMeshRenderer::markBoneExternal(
    const std::string& bone_name) {
  if (skinning_data_.bone_mapping.find(bone_name) ==
      skinning_data_.bone_mapping.end()) {
    throw std::runtime_error("AnimatedMeshRenderer '" + filename_ +
                             "' doesn't have any bone named '" + bone_name +
                             "'.");
  }

  // Find the bone that is to be marked
  aiNode* marked_node = findNode(scene_->mRootNode, bone_name);

  ExternalBoneTree ebone_tree(markChildExternal(nullptr, marked_node));

  // Get the root bone's BoneInfo
  size_t bidx = skinning_data_.bone_mapping[marked_node->mName.data];
  SkinningData::BoneInfo& binfo = skinning_data_.bone_info[bidx];

  // Set the root bone's local transformation
  // pointer to be able to set it from "inside".
  binfo.global_transform_ptr = ebone_tree.global_transform_ptr.get();
  binfo.pinned = true;

  return ebone_tree;
}

/// Returns the number of bones this scene has.
size_t AnimatedMeshRenderer::getNumBones() {
  // If loadBones hasn't been called yet, than have to create
  // the bones data first to know the number of bones.
  if (skinning_data_.per_mesh_attrib_max.size() == 0) {
    createBonesData();
  }

  return skinning_data_.num_bones;
}

/// Returns the size that boneIds and BoneWeights attribute arrays should be.
size_t AnimatedMeshRenderer::getBoneAttribNum() {
  // If loadBones hasn't been called yet, than have to create
  // the bones data first to know max_bone_attrib_num.
  if (skinning_data_.per_mesh_attrib_max.size() == 0) {
    createBonesData();
  }

  return skinning_data_.max_bone_attrib_num;
}

void AnimatedMeshRenderer::setupBones(gl::LazyVertexAttrib boneIDs,
                                      gl::LazyVertexAttrib bone_weights,
                                      bool integerIDs) {
if (!skinning_data_.is_setup_bones) {
    skinning_data_.is_setup_bones = true;
  } else {
    std::cerr << "AnimatedMeshRenderer::setupBones is called multiple times on "
                 "the same object. If the two calls want to set the bones up "
                 "into the same attribute position, then the second call is "
                 "unneccesary. If they want to set the bones to different "
                 "attribute positions then the second call would make the "
                 "first call not work anymore. Either way, calling "
                 "setupBones multiply times is a design error, that should "
                 "be avoided.";
    std::terminate();
  }

  // If the bones data hasn't been created yet, than call the function to do it.
  if (skinning_data_.per_mesh_attrib_max.size() == 0) {
    createBonesData();
  }

  if (skinning_data_.num_bones < std::numeric_limits<GLubyte>::max()) {
    shaderPlumbBones<GLubyte>(gl::IndexType::kUnsignedByte, boneIDs,
                              bone_weights, integerIDs);
  } else if (skinning_data_.num_bones < std::numeric_limits<GLushort>::max()) {
    shaderPlumbBones<GLushort>(gl::IndexType::kUnsignedShort, boneIDs,
                               bone_weights, integerIDs);
  } else {  // more than 65535 bones? WTF???
    shaderPlumbBones<GLuint>(gl::IndexType::kUnsignedInt, boneIDs,
                             bone_weights, integerIDs);
  }
}

}  // namespace engine
