// Copyright (c) 2014, Tamas Csala

#include <vector>
#include "./mesh_renderer.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {

/// Loads in the mesh from a file, and does some post-processing on it.
/** @param filename - The name of the file to load in.
  * @param flags - The assimp post-process flags. */
MeshRenderer::MeshRenderer(const std::string& filename,
                           gl::Bitfield<aiPostProcessSteps> flags)
    : scene_(importer_.ReadFile(filename.c_str(), flags|aiProcess_Triangulate))
    , filename_(filename)
    , entries_(scene_->mNumMeshes)
    , is_setup_positions_(false)
    , is_setup_normals_(false)
    , is_setup_tex_coords_(false)
    , textures_enabled_(true) {
  if (!scene_) {
    throw std::runtime_error("Error parsing " + filename_ + " : " +
                             importer_.GetErrorString());
  }

  // The world transform is the transform that takes the root node to it's
  // parent's space, which is the OpenGL style world space. The inverse of this
  // is stored as an attribute of the scene's root node.
  world_transformation_ =
    glm::inverse(engine::convertMatrix(scene_->mRootNode->mTransformation));
}

std::vector<int> MeshRenderer::btTriangles(btTriangleIndexVertexArray* triangles) {
  std::vector<int> indices_vector;

  for (unsigned mesh_idx = 0; mesh_idx < scene_->mNumMeshes; ++mesh_idx) {
    const aiMesh* mesh = scene_->mMeshes[mesh_idx];
    btIndexedMesh btMesh;
    btMesh.m_numVertices = mesh->mNumVertices;
    btMesh.m_vertexBase = (const unsigned char*)mesh->mVertices;
    btMesh.m_vertexStride = sizeof(aiVector3D);
    btMesh.m_vertexType = PHY_FLOAT;

    auto indices_begin_idx = indices_vector.size();
    indices_vector.reserve(indices_vector.size() + mesh->mNumFaces * 3);
    for (size_t face_idx = 0; face_idx < mesh->mNumFaces; face_idx++) {
      const aiFace& face = mesh->mFaces[face_idx];
      if (face.mNumIndices == 3) {  // The invalid faces are just ignored.
        indices_vector.push_back(face.mIndices[0]);
        indices_vector.push_back(face.mIndices[1]);
        indices_vector.push_back(face.mIndices[2]);
      }
    }
    btMesh.m_numTriangles = (indices_vector.size()-indices_begin_idx)/3;
    btMesh.m_triangleIndexBase = (const unsigned char*)&indices_vector[indices_begin_idx];
    btMesh.m_triangleIndexStride = 3*sizeof(int);
    btMesh.m_indexType = PHY_INTEGER;

    triangles->addIndexedMesh(btMesh, PHY_INTEGER);
  }

  return indices_vector;
}

/// Returns a vector of the vertices
std::vector<float> MeshRenderer::vertices() {
  std::vector<float> verts_vector;
  assert(3*sizeof(float) == sizeof(aiVector3D));

  for (unsigned mesh_idx = 0; mesh_idx < scene_->mNumMeshes; ++mesh_idx) {
    const aiMesh* mesh = scene_->mMeshes[mesh_idx];
    verts_vector.insert(verts_vector.end(), (float*)mesh->mVertices,
                        (float*)(mesh->mVertices+mesh->mNumVertices));
  }

  return verts_vector;
}

template <typename IdxType>
/// A template for setting different types (byte/short/int) of indices.
/** This expect the correct vao to be already bound!
  * @param index - The index of the entry */
void MeshRenderer::setIndices(size_t index) {
  const aiMesh* mesh = scene_->mMeshes[index];

  std::vector<IdxType> indices_vector;
  indices_vector.reserve(mesh->mNumFaces * 3);
  bool invalid_triangles = false;

  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    const aiFace& face = mesh->mFaces[i];
    if (face.mNumIndices == 3) {  // The invalid faces are just ignored.
      indices_vector.push_back(face.mIndices[0]);
      indices_vector.push_back(face.mIndices[1]);
      indices_vector.push_back(face.mIndices[2]);
    } else {
      invalid_triangles = true;
    }
  }

  if(invalid_triangles) {
    std::cerr << "Mesh '" << filename_ << "' contains non-triangle faces. "
                 "This might result in rendering artifacts." << std::endl;
  }

  gl::Bind(entries_[index].indices);
  entries_[index].indices.data(indices_vector);
  entries_[index].idx_count = indices_vector.size();
}

/// Loads in vertex positions and indices, and uploads the former into an attribute array.
/** Uploads the vertex positions data to an attribute array, and sets it up for use.
  * Calling this function changes the currently active VAO, ArrayBuffer and IndexBuffer.
  * The mesh cannot be drawn without calling this function.
  * @param attrib - The attribute array to use as destination. */
void MeshRenderer::setupPositions(gl::VertexAttrib attrib) {
  if (!is_setup_positions_) {
    is_setup_positions_ = true;
  } else {
    std::cerr << "MeshRenderer::setupPositions is called multiple times on the "
                 "same object. If the two calls want to set positions up into "
                 "the same attribute position, then the second call is "
                 "unneccesary. If they want to set the positions to different "
                 "attribute positions then the second call would make the "
                 "first call not work anymore. Either way, calling "
                 "setupPositions multiply times is a design error, that should "
                 "be avoided.";
    std::terminate();
  }

  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* mesh = scene_->mMeshes[i];
    gl::Bind(entries_[i].vao);

    // ~~~~~~<{ Load the vertices }>~~~~~~

    gl::Bind(entries_[i].verts);
    entries_[i].verts.data(mesh->mNumVertices*sizeof(aiVector3D), mesh->mVertices);
    attrib.setup<glm::vec3>().enable();

    // ~~~~~~<{ Load the indices }>~~~~~~

    if (mesh->mNumFaces * 3 < UCHAR_MAX) {
      entries_[i].idx_type = gl::kUnsignedByte;
      setIndices<unsigned char>(i);
    } else if (mesh->mNumFaces * 3 < USHRT_MAX) {
      entries_[i].idx_type = gl::kUnsignedShort;
      setIndices<unsigned short>(i);
    } else {
      entries_[i].idx_type = gl::kUnsignedInt;
      setIndices<unsigned int>(i);
    }
  }

  gl::Unbind(gl::kArrayBuffer);
  gl::Unbind(gl::kVertexArray);
}

/// Loads in vertex normals, and uploads it to an attribute array.
/** Uploads the vertex normals data to an attribute array, and sets it up for use.
  * Calling this function changes the currently active VAO and ArrayBuffer.
  * @param attrib - The attribute array to use as destination. */
void MeshRenderer::setupNormals(gl::VertexAttrib attrib) {
  if (!is_setup_normals_) {
    is_setup_normals_ = true;
  } else {
    std::cerr << "MeshRenderer::setupNormals is called multiple times on the "
                 "same object. If the two calls want to set normals up into "
                 "the same attribute position, then the second call is "
                 "unneccesary. If they want to set the normals to different "
                 "attribute positions then the second call would make the "
                 "first call not work anymore. Either way, calling "
                 "setupNormals multiply times is a design error, that should "
                 "be avoided.";
    std::terminate();
  }

  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* mesh = scene_->mMeshes[i];
    gl::Bind(entries_[i].vao);

    gl::Bind(entries_[i].normals);
    entries_[i].normals.data(mesh->mNumVertices*sizeof(aiVector3D), mesh->mNormals);
    attrib.setup<float>(3).enable();
  }

  gl::Unbind(gl::kArrayBuffer);
  gl::Unbind(gl::kVertexArray);
}

/// Checks if every mesh in the scene has tex_coords
/** Returns true if all of the meshes in the scene have texture
  * coordinates in the specified texture coordinate set.
  * @param tex_coord_set  Specifies the index of the texture coordinate
  *                     set that should be inspected */
bool MeshRenderer::hasTexCoords(unsigned char tex_coord_set) {
  for (size_t i = 0; i < entries_.size(); i++) {
    if (!scene_->mMeshes[i]->HasTextureCoords(tex_coord_set)) {
      return false;
    }
  }

  return true;
}

/// Loads in vertex texture coordinates (the 0th set), and the materials.
/** Uploads the vertex textures coordinates data to an attribute array,
  * and sets it up for use. Also loads in the materials (textures) for
  * the mesh. May write to the stderr if a material is missing.
  * Calling this function changes the currently active VAO and ArrayBuffer.
  * @param attrib - The attribute array to use as destination.
  * @param tex_coord_set  Specifies the index of the texture coordinate set
  *                     that should be used */
void MeshRenderer::setupTexCoords(gl::VertexAttrib attrib,
                                  unsigned char tex_coord_set) {
  if (!is_setup_tex_coords_) {
    is_setup_tex_coords_ = true;
  } else {
    std::cerr << "MeshRenderer::setupTexCoords is called multiple times on the "
                 "same object. If the two calls want to set tex_coords up into "
                 "the same attribute position, then the second call is "
                 "unneccesary. If they want to set the tex_coords to different "
                 "attribute positions then the second call would make the "
                 "first call not work anymore. Either way, calling "
                 "setupTexCoords multiply times is a design error, that should "
                 "be avoided.";
    std::terminate();
  }

  // Initialize TexCoords
  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* mesh = scene_->mMeshes[i];
    entries_[i].material_index = mesh->mMaterialIndex;

    std::vector<aiVector2D> tex_coords_vector;

    size_t vert_num = mesh->mNumVertices;
    if (mesh->HasTextureCoords(tex_coord_set)) {
      tex_coords_vector.reserve(vert_num);
      for (size_t i = 0; i < vert_num; i++) {
        const aiVector3D& tex_coord = mesh->mTextureCoords[tex_coord_set][i];
        tex_coords_vector.emplace_back(tex_coord.x, tex_coord.y);
      }
    } else {
      tex_coords_vector.resize(vert_num);
    }

    gl::Bind(entries_[i].vao);

    gl::Bind(entries_[i].tex_coords);
    entries_[i].tex_coords.data(tex_coords_vector);
    attrib.setup<float>(2).enable();
  }

  gl::Unbind(gl::kArrayBuffer);
  gl::Unbind(gl::kVertexArray);
}

#if OGLWRAP_USE_IMAGEMAGICK
/**
 * @brief Loads in a specified type of texture for every mesh. If no texture but
 *        a single color is specified, then sets up an 1x1 texture with that
 *        color (so you can use the same shader).
 *
 * Changes the currently active texture unit and Texture2D binding.
 * @param texture_unit      Specifies the texture unit to use for the textures.
 * @param tex_type          The type of the texture to load in. For ex
 *                          aiTextureType_DIFFUSE.
 * @param pKey, type, idx   These parameters identify the color parameter to
 *                          load in case there isn't any texture specified.
 *                          Use the assimp macros to fill these 3 parameters
 *                          all at once, for ex: AI_MATKEY_COLOR_DIFFUSE
 * @param srgb              Specifies weather the image is in srgb colorspace
 */
void MeshRenderer::setupTextures(unsigned short texture_unit,
                                 aiTextureType tex_type,
                                 const char *pKey,
                                 unsigned int type,
                                 unsigned int idx,
                                 bool srgb) {
  gl::ActiveTexture(texture_unit);

  materials_[tex_type].active = true;
  materials_[tex_type].tex_unit = texture_unit;

  if (scene_->mNumMaterials) {
    // Extract the directory part from the file name
    std::string::size_type slash_idx = filename_.find_last_of("/");
    std::string dir;

    if (slash_idx == std::string::npos) {
      dir = "./";
    } else if (slash_idx == 0) {
      dir = "/";
    } else {
      dir = filename_.substr(0, slash_idx + 1);
    }

    // Initialize the materials
    for (unsigned int i = 0; i < scene_->mNumMaterials; ++i) {
      const aiMaterial* mat = scene_->mMaterials[i];
      materials_[tex_type].textures.push_back(gl::Texture2D{});

      aiString filepath;
      if (mat->GetTexture(tex_type, 0, &filepath) == AI_SUCCESS) {
        gl::Bind(materials_[tex_type].textures[i]);
        materials_[tex_type].textures[i].loadTexture(dir + filepath.data,
                                                     srgb ? "CSRGBA" : "CRGBA");
        materials_[tex_type].textures[i].minFilter(gl::kLinear);
        materials_[tex_type].textures[i].magFilter(gl::kLinear);
      } else {
        aiColor4D color(0.f, 0.f, 0.f, 1.0f);
        mat->Get(pKey, type, idx, color);

        gl::Bind(materials_[tex_type].textures[i]);
        materials_[tex_type].textures[i].upload(gl::kRgba32F, 1, 1, gl::kRgba,
                                                gl::kFloat, &color.r);
        materials_[tex_type].textures[i].minFilter(gl::kNearest);
        materials_[tex_type].textures[i].magFilter(gl::kNearest);
      }
    }
  }

  gl::Unbind(gl::kTexture2D);
}
#endif

/// Sets the diffuse textures up to a specified texture unit.
/** Changes the currently active texture unit and Texture2D binding.
  * @param texture_unit Specifies the texture unit to use for the diffuse textures. */
void MeshRenderer::setupDiffuseTextures(unsigned short texture_unit, bool srbg) {
  setupTextures(texture_unit, aiTextureType_DIFFUSE,
                AI_MATKEY_COLOR_DIFFUSE, srbg);
}

/// Sets the specular textures up to a specified texture unit.
/** Changes the currently active texture unit and Texture2D binding.
  * @param texture_unit Specifies the texture unit to use for the specular textures. */
void MeshRenderer::setupSpecularTextures(unsigned short texture_unit) {
  setupTextures(texture_unit, aiTextureType_SPECULAR,
                AI_MATKEY_COLOR_SPECULAR, false);
}

/// Renders the mesh.
/** Changes the currently active VAO and may change the Texture2D binding */
void MeshRenderer::render() {
  if (!is_setup_positions_) {
    return;  // we can't render the mesh, if we don't have any vertex.
  }
  for (size_t i = 0 ; i < entries_.size(); i++) {
    gl::Bind(entries_[i].vao);

    const size_t material_index = entries_[i].material_index;

    if (textures_enabled_) {
      for (auto iter = materials_.begin(); iter != materials_.end(); iter++) {
        auto& material = iter->second;
        if (material.active == true && material_index < scene_->mNumMaterials) {
          gl::ActiveTexture(material.tex_unit);
        }
        gl::Bind(material.textures[material_index]);
      }
    }

    gl::DrawElements(gl::kTriangles, entries_[i].idx_count, entries_[i].idx_type);

    if (textures_enabled_) {
      for (auto iter = materials_.begin(); iter != materials_.end(); iter++) {
        auto& material = iter->second;
        if (material.active == true && material_index < scene_->mNumMaterials) {
          gl::ActiveTexture(material.tex_unit);
        }
        gl::Unbind(material.textures[material_index]);
      }
    }
  }

  gl::Unbind(gl::kVertexArray);
}

/// The transformation that takes the model's world coordinates to the OpenGL style world coordinates.
/** i.e if you see that a character is laying on ground instead of standing, it is probably
  * because the character is defined in a space where XY is flat, and Z is up. Right
  * multiplying your model matrix with this matrix will solve that problem. */
glm::mat4 MeshRenderer::worldTransform() const {
  return world_transformation_;
}

/// Gives information about the mesh's bounding cuboid.
BoundingBox MeshRenderer::boundingBox(const glm::mat4& matrix) const {
  float zero = 0.0f;  // This is needed to bypass a visual c++ compile error
  float infty = 1.0f / zero;
  glm::vec3 mins{infty, infty, infty}, maxes{-infty, -infty, -infty};
  for (size_t entry = 0; entry < entries_.size(); entry++) {
    const aiMesh* mesh = scene_->mMeshes[entry];

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
      glm::vec4 vert {mesh->mVertices[i].x, mesh->mVertices[i].y,
                      mesh->mVertices[i].z, 1};
      vert = matrix * vert;

      if (vert.x < mins.x) {
        mins.x = vert.x;
      }
      if (vert.y < mins.y) {
        mins.y = vert.y;
      }
      if (vert.z < mins.z) {
        mins.z = vert.z;
      }

      if (maxes.x < vert.x) {
        maxes.x = vert.x;
      }
      if (maxes.y < vert.y) {
        maxes.y = vert.y;
      }
      if (maxes.z < vert.z) {
        maxes.z = vert.z;
      }
    }
  }

  return BoundingBox{mins, maxes};
}

glm::vec4 MeshRenderer::bSphere(const BoundingBox& bbox) const {
  glm::vec3 center = bbox.center(), extent = bbox.extent();
  return glm::vec4(center, sqrt(glm::dot(extent, extent)) / 2);  // Pythagoras.
}

/// Returns the center offseted by the model matrix (as xyz) and radius (as w) of the bounding sphere.
/** @param model_matrix - The matrix to use to offset the center of the bounding sphere. */
glm::vec4 MeshRenderer::bSphere(const glm::mat4& model_matrix) const {
  return bSphere(boundingBox(model_matrix));
}

/// Returns the center of the bounding sphere.
glm::vec3 MeshRenderer::bSphereCenter() const {
  return boundingBox().center();
}

/// Returns the radius of the bounding sphere.
float MeshRenderer::bSphereRadius() const {
  glm::vec3 extent = boundingBox().extent();
  return sqrt(glm::dot(extent, extent)) / 2;  // Pythagoras.
}

}  // namespace engine
