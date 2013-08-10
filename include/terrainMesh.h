#ifndef HEADER_A71A3471183A48FA
#define HEADER_A71A3471183A48FA

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/texture.hpp"

#include "terrainData.h"

// Selecting the Block's size (is 2 ^ (blockMipmapLevel + 1)) is really essential
// but not trivial. Selecting the good number can prove you about twice as much
// performance as with selecting a number 1 higher or 1 lower number, and it won't
// just be faster, but it will also look better. So everytime you try a new mesh,
// tweak this value, the optimum should be near ceil(log2(texSize) / 2).

#define blockMipmapLevel 7
const int blockRadius = 1 << blockMipmapLevel;

class TerrainMesh {
    oglwrap::VertexArray vao[blockMipmapLevel];
    oglwrap::Buffer positions[blockMipmapLevel];
    oglwrap::IndexBuffer indices[blockMipmapLevel], borderIndices[blockMipmapLevel][6][2];
    size_t vertexNum[blockMipmapLevel], indexNum[blockMipmapLevel];
    oglwrap::Texture2D colorMap, heightMap, normalMap;

    TerrainData terrain;
    size_t w, h;
    RawImageData image;

    void DrawBlocks(const glm::vec3& camPos,
                    oglwrap::LazyUniform<glm::vec2>& Offset
    );
    void CreateConnectors(glm::vec2& pos, glm::vec2& camPos);
public:
    TerrainMesh(const std::string& terrainFile,
                const std::string& diffusePicture
    );
    void Render(const glm::vec3& camPos,
                oglwrap::LazyUniform<glm::vec2>& Offset,
                oglwrap::LazyUniform<glm::vec3>& scales
    );
};


#endif // header guard
