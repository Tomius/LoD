// Copyright (c) 2014, Tamas Csala

#include "mesh_renderer.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/smart_enums.h"

namespace engine {

/// Loads in the mesh from a file, and does some post-processing on it.
/** @param filename - The name of the file to load in.
  * @param flags - The assimp post-process flags. */
MeshRenderer::MeshRenderer(const std::string& filename,
                           gl::Bitfield<aiPostProcessSteps> flags)
  : scene_(importer_.ReadFile(
             filename.c_str(),
             flags | aiProcess_Triangulate
           ))
  , filename_(filename)
  , entries_(scene_->mNumMeshes)
  , is_setup_positions_(false)
  , is_setup_normals_(false)
  , is_setup_texcoords_(false)
  , textures_enabled_(true) {

  if (!scene_) {
    throw std::runtime_error(
      "Error parsing " + filename_ + " : " + importer_.GetErrorString()
    );
  }

  // The world transform is the transform that takes the root node to it's
  // parent's space, which is the OpenGL style world space. The inverse of this
  // is stored as an attribute of the scene's root node.
  world_transformation_ =
    glm::inverse(engine::convertMatrix(scene_->mRootNode->mTransformation));
}

template <typename IdxType>
/// A template for setting different types (byte/short/int) of indices.
/** This expect the correct vao to be already bound!
  * @param index - The index of the entry */
void MeshRenderer::setIndices(size_t index) {
  const aiMesh* paiMesh = scene_->mMeshes[index];

  std::vector<IdxType> indicesVector;
  indicesVector.reserve(paiMesh->mNumFaces * 3);

  for (size_t i = 0; i < paiMesh->mNumFaces; i++) {
    const aiFace& face = paiMesh->mFaces[i];
    if (face.mNumIndices == 3) { // The invalid vertices are just ignored.
      indicesVector.push_back(face.mIndices[0]);
      indicesVector.push_back(face.mIndices[1]);
      indicesVector.push_back(face.mIndices[2]);
    }
  }

  entries_[index].indices.bind();
  entries_[index].indices.data(indicesVector);
  entries_[index].idxCount = indicesVector.size();
}

/// Loads in vertex positions and indices, and uploads the former into an attribute array.
/** Uploads the vertex positions data to an attribute array, and sets it up for use.
  * Calling this function changes the currently active VAO, ArrayBuffer and IndexBuffer.
  * The mesh cannot be drawn without calling this function.
  * @param attrib - The attribute array to use as destination. */
void MeshRenderer::setupPositions(gl::VertexAttribArray attrib) {
  if (is_setup_positions_) {
    throw std::logic_error(
      "MeshRenderer::setup_position is called multiply times on the same object"
    );
  } else {
    is_setup_positions_ = true;
  }

  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* paiMesh = scene_->mMeshes[i];

    // ~~~~~~<{ Load the vertices }>~~~~~~

    std::vector<aiVector3D> vertsVector;
    size_t vertNum = paiMesh->mNumVertices;
    vertsVector.reserve(vertNum);

    for (size_t i = 0; i < vertNum; i++) {
      vertsVector.push_back(paiMesh->mVertices[i]);
    }

    entries_[i].vao.bind();

    entries_[i].verts.bind();
    entries_[i].verts.data(vertsVector);
    attrib.setup<float>(3).enable();

    // ~~~~~~<{ Load the indices }>~~~~~~

    if (paiMesh->mNumFaces * 3 < UCHAR_MAX) {
      entries_[i].idxType = gl::kUnsignedByte;
      setIndices<unsigned char>(i);
    } else if (paiMesh->mNumFaces * 3 < USHRT_MAX) {
      entries_[i].idxType = gl::kUnsignedShort;
      setIndices<unsigned short>(i);
    } else {
      entries_[i].idxType = gl::kUnsignedInt;
      setIndices<unsigned int>(i);
    }
  }

  gl::VertexArray::Unbind();
  gl::ArrayBuffer::Unbind();
}

/// Loads in vertex normals, and uploads it to an attribute array.
/** Uploads the vertex normals data to an attribute array, and sets it up for use.
  * Calling this function changes the currently active VAO and ArrayBuffer.
  * @param attrib - The attribute array to use as destination. */
void MeshRenderer::setupNormals(gl::VertexAttribArray attrib) {

  if (is_setup_normals_) {
    throw std::logic_error(
      "MeshRenderer::setupNormals is called multiply times on the same object"
    );
  } else {
    is_setup_normals_ = true;
  }

  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* paiMesh = scene_->mMeshes[i];

    std::vector<aiVector3D> normalsVector;

    size_t vertNum = paiMesh->mNumVertices;
    normalsVector.reserve(vertNum);

    for (size_t i = 0; i < vertNum; i++) {
      normalsVector.push_back(paiMesh->mNormals[i]);
    }

    entries_[i].vao.bind();

    entries_[i].normals.bind();
    entries_[i].normals.data(normalsVector);
    attrib.setup<float>(3).enable();
  }

  gl::VertexArray::Unbind();
  gl::ArrayBuffer::Unbind();
}

/// Checks if every mesh in the scene has texcoords
/** Returns true if all of the meshes in the scene have texture
  * coordinates in the specified texture coordinate set.
  * @param texCoordSet  Specifies the index of the texture coordinate
  *                     set that should be inspected */
bool MeshRenderer::hasTexCoords(unsigned char texCoordSet) {
  for (size_t i = 0; i < entries_.size(); i++) {
    if (!scene_->mMeshes[i]->HasTextureCoords(texCoordSet)) {
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
  * @param texCoordSet  Specifies the index of the texture coordinate set
  *                     that should be used */
void MeshRenderer::setupTexCoords(gl::VertexAttribArray attrib,
                                  unsigned char texCoordSet) {

  if (is_setup_texcoords_) {
    throw std::logic_error(
      "MeshRenderer::setupTexCoords is called multiply times on the same object"
    );
  } else {
    is_setup_texcoords_ = true;
  }

  // Initialize TexCoords
  for (size_t i = 0; i < entries_.size(); i++) {
    const aiMesh* paiMesh = scene_->mMeshes[i];
    entries_[i].materialIndex = paiMesh->mMaterialIndex;

    std::vector<aiVector2D> texCoordsVector;

    size_t vertNum = paiMesh->mNumVertices;
    if (paiMesh->HasTextureCoords(texCoordSet)) {
      texCoordsVector.reserve(vertNum);
      for (size_t i = 0; i < vertNum; i++) {
        const aiVector3D& texC = paiMesh->mTextureCoords[texCoordSet][i];
        texCoordsVector.push_back(aiVector2D(texC.x, texC.y));
      }
    } else {
      texCoordsVector.resize(vertNum);
    }

    entries_[i].vao.bind();

    entries_[i].texCoords.bind();
    entries_[i].texCoords.data(texCoordsVector);
    attrib.setup<float>(2).enable();
  }

  gl::VertexArray::Unbind();
  gl::ArrayBuffer::Unbind();
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
  gl::Texture2D::Active(texture_unit);

  materials_[tex_type].active = true;
  materials_[tex_type].texUnit = texture_unit;
  materials_[tex_type].textures.resize(scene_->mNumMaterials);

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
    for (unsigned int i = 0; i < scene_->mNumMaterials; i++) {
      const aiMaterial* mat = scene_->mMaterials[i];

      aiString filepath;
      if (mat->GetTexture(tex_type, 0, &filepath) == AI_SUCCESS) {
        materials_[tex_type].textures[i].bind();
        materials_[tex_type].textures[i].loadTexture(dir + filepath.data,
                                                     srgb ? "CSRGBA" : "CRGBA");
        materials_[tex_type].textures[i].minFilter(gl::kLinear);
        materials_[tex_type].textures[i].magFilter(gl::kLinear);
      } else {
        aiColor4D color(0.f, 0.f, 0.f, 1.0f);
        mat->Get(pKey, type, idx, color);

        materials_[tex_type].textures[i].bind();
        materials_[tex_type].textures[i].upload(gl::kRgba32F, 1, 1, gl::kRgba,
                                                gl::kFloat, &color.r);
        materials_[tex_type].textures[i].minFilter(gl::kNearest);
        materials_[tex_type].textures[i].magFilter(gl::kNearest);
      }
    }
  }

  gl::Texture2D::Unbind();
}
#endif

/// Sets the diffuse textures up to a specified texture unit.
/** Changes the currently active texture unit and Texture2D binding.
  * @param texture_unit Specifies the texture unit to use for the diffuse textures. */
void MeshRenderer::setupDiffuseTextures(unsigned short texture_unit) {
  setupTextures(texture_unit, aiTextureType_DIFFUSE, AI_MATKEY_COLOR_DIFFUSE);
}

/// Sets the specular textures up to a specified texture unit.
/** Changes the currently active texture unit and Texture2D binding.
  * @param texture_unit Specifies the texture unit to use for the specular textures. */
void MeshRenderer::setupSpecularTextures(unsigned short texture_unit) {
  setupTextures(texture_unit, aiTextureType_SPECULAR, AI_MATKEY_COLOR_SPECULAR, false);
}

/// Renders the mesh.
/** Changes the currently active VAO and may change the Texture2D binding */
void MeshRenderer::render() {
  if (!is_setup_positions_) {
    return;
  }
  for (size_t i = 0 ; i < entries_.size(); i++) {
    entries_[i].vao.bind();

    const size_t materialIndex = entries_[i].materialIndex;

    if (textures_enabled_) {
      for (auto iter = materials_.begin(); iter != materials_.end(); iter++) {
        auto& material = iter->second;
        if (material.active == true && materialIndex < scene_->mNumMaterials) {
          material.textures[materialIndex].active(material.texUnit);
        }
        material.textures[materialIndex].bind();
      }
    }

    gl::DrawElements(gl::kTriangles, entries_[i].idxCount, entries_[i].idxType);

    if (textures_enabled_) {
      for (auto iter = materials_.begin(); iter != materials_.end(); iter++) {
        auto& material = iter->second;
        if (material.active == true && materialIndex < scene_->mNumMaterials) {
          material.textures[materialIndex].active(material.texUnit);
        }
        material.textures[materialIndex].unbind();
      }
    }
  }

  gl::VertexArray::Unbind();
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
  // Idea: get the minimums and maximums of the vertex positions
  // in each coordinate. Then the average of the mins and maxes
  // will be the center of the cuboid
  float infty = 1.0f / 0.0f;
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

/// Returns the center (as xyz) and radius (as w) of the bounding sphere.
glm::vec4 MeshRenderer::bSphere() const {
  BoundingBox bbox = boundingBox();
  glm::vec3 center = bbox.center(), extent = bbox.extent();
  return glm::vec4(center, sqrt(glm::dot(extent, extent)) / 2); // Pythagoras.
}

/// Returns the center offseted by the model matrix (as xyz) and radius (as w) of the bounding sphere.
/** @param modelMatrix - The matrix to use to offset the center of the bounding sphere. */
glm::vec4 MeshRenderer::bSphere(const glm::mat4& modelMatrix) const {
  glm::vec4 m_bSphere = bSphere();
  return glm::vec4(glm::vec3(modelMatrix *
                   glm::vec4(glm::vec3(m_bSphere), 1)), m_bSphere.w);
}

/// Returns the center of the bounding sphere.
glm::vec3 MeshRenderer::bSphereCenter() const {
  return boundingBox().center();
}

/// Returns the radius of the bounding sphere.
float MeshRenderer::bSphereRadius() const {
  glm::vec3 extent = boundingBox().extent();
  return sqrt(glm::dot(extent, extent)) / 2; // Pythagoras.
}

} // namespace engine
