#ifndef HEADER_A71A3471183A48FA
#define HEADER_A71A3471183A48FA

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"

#include "terrainData.h"
//#include "svec2.hpp"

// Selecting the Blocks' size is really essential but not trivial. Selecting the
// good number can prove you about twice as much performance as with selecting a
// number 1 higher or 1 lower number, and it won't just be faster, but it will
// also look better. So everytime you try a new mesh, tweak this value, the
// optimum should be near ceil(log2(texSize) / 2).

#define blockMipmapLevel 7
const int blockRadius = 1 << blockMipmapLevel;

class TerrainMesh {
    oglwrap::VertexArray vao[blockMipmapLevel];
    oglwrap::ArrayBuffer positions[blockMipmapLevel];
    oglwrap::IndexBuffer indices[blockMipmapLevel], borderIndices[blockMipmapLevel][6][2];
    size_t vertexNum[blockMipmapLevel], indexNum[blockMipmapLevel];
    oglwrap::Texture2D colorMap, heightMap, grassMap, grassNormalMap;

    RawTerrainData terrain;
    size_t w, h;
    RawImageData image;

    void DrawBlocks(const glm::vec3& camPos,
                    oglwrap::LazyUniform<glm::ivec2>& Offset,
                    oglwrap::LazyUniform<int>& mipmapLevelUnif
    );
    void CreateConnectors(glm::ivec2 pos, glm::vec2 camPos);
public:
    TerrainMesh(const std::string& terrainFile,
                const std::string& diffusePicture
    );

    void render(const glm::vec3& camPos,
                oglwrap::LazyUniform<glm::ivec2>& Offset,
                oglwrap::LazyUniform<int>& mipmapLevel
    );

    unsigned char FetchHeight(glm::ivec2 v);
    float getHeight(float x, float y);
};


#endif // header guard
