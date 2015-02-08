// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MESH_MESH_RENDERER_H_
#define ENGINE_MESH_MESH_RENDERER_H_

#include <map>
#include <memory>
#include <climits>
#include <btBulletDynamicsCommon.h>

#include "../oglwrap_config.h"
#include "../../oglwrap/buffer.h"
#include "../../oglwrap/enums/index_type.h"
#include "../../oglwrap/vertex_attrib.h"
#include "../../oglwrap/textures/texture_2D.h"

#include "../assimp.h"
#include "../collision/bounding_box.h"

namespace engine {

/// A class that can load in and draw meshes using assimp.
class MeshRenderer {
 protected:
  /**
   * @brief A class to store per mesh data (the class loads in a scene, that
   *        might contain multiply meshes).
   */
  struct MeshEntry {
    gl::VertexArray vao;
    gl::ArrayBuffer verts, normals, tex_coords;
    gl::IndexBuffer indices;
    unsigned idx_count, material_index;
    static const unsigned kInvalidMaterial = unsigned(-1);
    gl::IndexType idx_type;

    MeshEntry() : material_index(kInvalidMaterial) {}
  };

  /// The assimp importer. The scene actually belongs to this.
  Assimp::Importer importer_;

  /// A pointer to the scene stored by the importer. But this is the working interface for it.
  const aiScene* scene_;

  /// The name of the file loaded in. It is stored to be able to print it out if an error happens.
  std::string filename_;

  /// The vao-s and buffers per mesh.
  std::vector<MeshEntry> entries_;

  /// The transformation that takes the model's world coordinates to the OpenGL style world coordinates.
  glm::mat4 world_transformation_;

  /// A struct containin the state and data of a material type.
  struct MaterialInfo {
    bool active;
    int tex_unit;
    std::vector<gl::Texture2D> textures;

    MaterialInfo() : active(false), tex_unit(0) {}
  };

  /// The materials.
  std::map<aiTextureType, MaterialInfo> materials_;

  /// Stores if the setupPositions function is called (they shouldn't be called more than once).
  bool is_setup_positions_;
  /// Stores if the setupNormals function is called (they shouldn't be called more than once).
  bool is_setup_normals_;
  /// Stores if the setup_texCoords function is called (they shouldn't be called more than once).
  bool is_setup_tex_coords_;
  /// Textures can be disabled, and not used for rendering
  bool textures_enabled_;

  /// It shouldn't be copyable.
  MeshRenderer(const MeshRenderer& src) = delete;
  /// It shouldn't be copyable.
  void operator=(const MeshRenderer& rhs) = delete;

public:
  /// Loads in the mesh from a file, and does some post-processing on it.
  /** @param filename - The name of the file to load in.
    * @param flags - The assimp post-process flags. */
  MeshRenderer(const std::string& filename,
               gl::Bitfield<aiPostProcessSteps> flags);

  template <typename IdxType>
  /// Returns a vector of the indices
  std::vector<IdxType> indices();

  /// Returns a vector of the vertices
  std::vector<float> vertices();

  /// Sets up a btTriangleIndexVertexArray, and returns a vector of indices
  /// that should be stored throughout the lifetime of the bullet object
  std::vector<int> btTriangles(btTriangleIndexVertexArray* triangles);

private:
  template <typename IdxType>
  /// A template for setting different types (byte/short/int) of indices.
  /** This expects the correct vao to be already bound!
    * @param index - The index of the entry */
  void setIndices(size_t index);

public:
  /// Loads in vertex positions and indices, and uploads the former into an attribute array.
  /** Uploads the vertex positions data to an attribute array, and sets it up for use.
    * Calling this function changes the currently active VAO, ArrayBuffer and IndexBuffer.
    * The mesh cannot be drawn without calling this function.
    * @param attrib - The attribute array to use as destination. */
  void setupPositions(gl::VertexAttrib attrib);

  /// Loads in vertex normals, and uploads it to an attribute array.
  /** Uploads the vertex normals data to an attribute array, and sets it up for use.
    * Calling this function changes the currently active VAO and ArrayBuffer.
    * @param attrib - The attribute array to use as destination. */
  void setupNormals(gl::VertexAttrib attrib);

  /// Checks if every mesh in the scene has tex_coords
  /** Returns true if all of the meshes in the scene have texture
    * coordinates in the specified texture coordinate set.
    * @param tex_coord_set - Specifies the index of the texture coordinate set that should be inspected */
  bool hasTexCoords(unsigned char tex_coord_set = 0);

  /// Loads in vertex texture coordinates (the 0th set), and the materials.
  /** Uploads the vertex textures coordinates data to an attribute array,
    * and sets it up for use. Also loads in the materials (textures) for
    * the mesh. May write to the stderr if a material is missing.
    * Calling this function changes the currently active VAO and ArrayBuffer.
    * @param attrib - The attribute array to use as destination.
    * @param tex_coord_set Specifies the index of the texture coordinate set that should be used */
  void setupTexCoords(gl::VertexAttrib attrib,
                      unsigned char tex_coord_set = 0);

  /**
   * @brief Loads in a specified type of texture for every mesh. If no texture
   *        but a single color is specified, then sets up an 1x1 texture with
   *        that color (so you can use the same shader).
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
  void setupTextures(unsigned short texture_unit,
                     aiTextureType tex_type,
                     const char *pKey,
                     unsigned int type,
                     unsigned int idx,
                     bool srgb = true);

  /// Sets the diffuse textures up to a specified texture unit.
  /** Changes the currently active texture unit and Texture2D binding.
    * @param texture_unit - Specifies the texture unit to use for the diffuse textures. */
  void setupDiffuseTextures(unsigned short texture_unit, bool srbg = true);

  /// Sets the specular textures up to a specified texture unit.
  /** Changes the currently active texture unit and Texture2D binding.
    * @param texture_unit - Specifies the texture unit to use for the specular textures. */
  void setupSpecularTextures(unsigned short texture_unit);

  /// Renders the mesh.
  /** Changes the currently active VAO and may change the Texture2D binding */
  void render();

  /// Gives information about the mesh's bounding cuboid.
  BoundingBox boundingBox(const glm::mat4& matrix = glm::mat4{}) const;

  /// Returns the transformation that takes the model's world coordinates to the OpenGL style world coordinates.
  /** i.e if you see that a character is laying on ground instead of standing, it is probably
    * because the character is defined in a space where Z is up not Y. Right multiplying your
    * model matrix with this matrix will solve that problem. */
  glm::mat4 worldTransform() const;

  /// Returns the bounding sphere from the bounding box
  glm::vec4 bSphere(const BoundingBox& bbox) const;

  /// Returns the center offseted by the model matrix (as xyz) and radius (as w) of the bounding sphere.
  glm::vec4 bSphere(const glm::mat4& modelMatrix = glm::mat4{}) const;

  /// Returns the center of the bounding sphere.
  glm::vec3 bSphereCenter() const;

  /// Returns the radius of the bounding sphere.
  float bSphereRadius() const;

  /// Enables the use of textures for rendering.
  void enableTextures() { textures_enabled_ = true; }

  /// Disables the use of textures for rendering.
  void disableTextures() { textures_enabled_ = true; }
};

}  // namespace engine

#include "./mesh_renderer-inl.h"

#endif  // ENGINE_MESH_MESH_RENDERER_H_
