/** @file mesh_renderer.hpp
    @brief A Mesh loader and renderer using assimp
*/

#ifndef ENGINE_MESH_MESH_RENDERER_H_
#define ENGINE_MESH_MESH_RENDERER_H_

#include <map>
#include <memory>
#include <climits>

#include "oglwrap/config.h"
#include "oglwrap/general.h"
#include "oglwrap/buffer.h"
#include "oglwrap/enums/index_type.h"
#include "oglwrap/vertex_attrib.h"
#include "oglwrap/textures/texture_2D.h"
#include "../assimp.h"

namespace engine {

/// A class that can load in and draw meshes from over 30 file formats using assimp.
class MeshRenderer {
protected:

  /// A class to store per mesh data (the class loads in a scene, that might contain multiply meshes).
  struct MeshEntry {
    oglwrap::VertexArray vao;
    oglwrap::ArrayBuffer verts, normals, texCoords;
    oglwrap::IndexBuffer indices;
    unsigned idxCount, materialIndex;
    oglwrap::IndexType idxType;

    MeshEntry() : materialIndex(0xFFFFFFFF) {} // Invalid material
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
    int texUnit;
    std::vector<oglwrap::Texture2D> textures;


    MaterialInfo()
      : active(false), texUnit(0) {
    }
  };

  /// The materials.
  std::map<aiTextureType, MaterialInfo> materials_;

  /// Stores if the setupPositions function is called (they shouldn't be called more than once).
  bool is_setup_positions_;
  /// Stores if the setupNormals function is called (they shouldn't be called more than once).
  bool is_setup_normals_;
  /// Stores if the setup_texcoords function is called (they shouldn't be called more than once).
  bool is_setup_texcoords_;
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
               oglwrap::Bitfield<aiPostProcessSteps> flags);

private:
  template <class IdxType>
  /// A template for setting different types (byte/short/int) of indices.
  /** This expect the correct vao to be already bound!
    * @param index - The index of the entry */
  void setIndices(size_t index);

public:
  /// Loads in vertex positions and indices, and uploads the former into an attribute array.
  /** Uploads the vertex positions data to an attribute array, and sets it up for use.
    * Calling this function changes the currently active VAO, ArrayBuffer and IndexBuffer.
    * The mesh cannot be drawn without calling this function.
    * @param attrib - The attribute array to use as destination. */
  void setupPositions(oglwrap::VertexAttribArray attrib);

  /// Loads in vertex normals, and uploads it to an attribute array.
  /** Uploads the vertex normals data to an attribute array, and sets it up for use.
    * Calling this function changes the currently active VAO and ArrayBuffer.
    * @param attrib - The attribute array to use as destination. */
  void setupNormals(oglwrap::VertexAttribArray attrib);

  /// Checks if every mesh in the scene has texcoords
  /** Returns true if all of the meshes in the scene have texture
    * coordinates in the specified texture coordinate set.
    * @param texCoordSet - Specifies the index of the texture coordinate set that should be inspected */
  bool hasTexCoords(unsigned char texCoordSet = 0);

  /// Loads in vertex texture coordinates (the 0th set), and the materials.
  /** Uploads the vertex textures coordinates data to an attribute array,
    * and sets it up for use. Also loads in the materials (textures) for
    * the mesh. May write to the stderr if a material is missing.
    * Calling this function changes the currently active VAO and ArrayBuffer.
    * @param attrib - The attribute array to use as destination.
    * @param texCoordSet Specifies the index of the texture coordinate set that should be used */
  void setupTexCoords(oglwrap::VertexAttribArray attrib,
                      unsigned char texCoordSet = 0);

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
   */
  void setupTextures(unsigned short texture_unit,
                     aiTextureType tex_type,
                     const char *pKey,
                     unsigned int type,
                     unsigned int idx);

  /// Sets the diffuse textures up to a specified texture unit.
  /** Changes the currently active texture unit and Texture2D binding.
    * @param texture_unit - Specifies the texture unit to use for the diffuse textures. */
  void setupDiffuseTextures(unsigned short texture_unit);

  /// Sets the specular textures up to a specified texture unit.
  /** Changes the currently active texture unit and Texture2D binding.
    * @param texture_unit - Specifies the texture unit to use for the specular textures. */
  void setupSpecularTextures(unsigned short texture_unit);

  /// Renders the mesh.
  /** Changes the currently active VAO and may change the Texture2D binding */
  void render();

  /// Gives information about the mesh's bounding cuboid.
  /** @param center - The vec3 where bounding cuboid's center is to be returned.
    * @param edges - The vec3 where bounding cuboid's edge lengths are to be returned. */
  void bCuboid(glm::vec3& center, glm::vec3& edges) const;

  /// Returns the transformation that takes the model's world coordinates to the OpenGL style world coordinates.
  /** i.e if you see that a character is laying on ground instead of standing, it is probably
    * because the character is defined in a space where Z is up not Y. Right multiplying your
    * model matrix with this matrix will solve that problem. */
  glm::mat4 worldTransform() const;

  /// Returns the center (as xyz) and radius (as w) of the bounding sphere.
  glm::vec4 bSphere() const;

  /// Returns the center offseted by the model matrix (as xyz) and radius (as w) of the bounding sphere.
  glm::vec4 bSphere(const glm::mat4& modelMatrix) const;

  /// Returns the center of the bounding sphere.
  glm::vec3 bSphereCenter() const;

  /// Returns the radius of the bounding sphere.
  float bSphereRadius() const;

  /// Enables the use of textures for rendering.
  void enableTextures() { textures_enabled_ = true; }

  /// Disables the use of textures for rendering.
  void disableTextures() { textures_enabled_ = true; }

}; // MeshRenderer class

} // namespace engine

#include "mesh_renderer-inl.h"

#endif // ENGINE_MESH_MESH_RENDERER_H_
