#ifndef LOD_TERRAINMESH_HPP_
#define LOD_TERRAINMESH_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/textures/texture_2D.hpp"

#include "terrainData.hpp"

// Selecting the Blocks' size is really essential but not trivial. Selecting the
// good number can prove you about twice as much performance as with selecting a
// number 1 higher or 1 lower number, and it won't just be faster, but it will
// also look better. So everytime you try a new mesh, tweak this value, the
// optimum should be near ceil(log2(texSize) / 2).

const int kBlockMipmapLevel = 7;
const int kBlockRadius = 1 << kBlockMipmapLevel;

class TerrainMesh {
  oglwrap::VertexArray vao_[kBlockMipmapLevel];
  oglwrap::ArrayBuffer positions_[kBlockMipmapLevel];
  oglwrap::IndexBuffer indices_[kBlockMipmapLevel], border_indices_[kBlockMipmapLevel][6][2];
  size_t index_num_[kBlockMipmapLevel];
  oglwrap::Texture2D heightMap_, grassMaps_[2], grassNormalMap_, grassBumpMap_;

  RawTerrainData terrain_;

  void DrawBlocks(const glm::vec3& camPos,
                  const glm::vec3& _camFwd,
                  oglwrap::LazyUniform<glm::ivec2>& Offset,
                  oglwrap::LazyUniform<int>& uMipmapLevel_
                 );
  void CreateConnectors(glm::ivec2 pos, glm::vec2 camPos);

  int w_, h_;
public:
  const int& w, h;

  TerrainMesh(const std::string& terrainFile);

  void render(const glm::vec3& camPos,
              const glm::vec3& camFwd,
              oglwrap::LazyUniform<glm::ivec2>& Offset,
              oglwrap::LazyUniform<int>& uMipmapLevel_
             );

  unsigned char fetchHeight(glm::ivec2 v) const;
  double getHeight(double x, double y) const;
};

#endif // LOD_TERRAINMESH_HPP_
