#pragma once

#if defined(__APPLE__)
    #include <OpenGL/glew.h>
#else
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
        #include <windows.h>
    #endif
    #include <GL/glew.h>
#endif

#include "oglwrap/oglwrap.hpp"

#include "terrainData.h"

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
    oglwrap::Texture2D heightMap, grassMaps[2], grassNormalMap, grassBumpMap;

    RawTerrainData terrain;

    void DrawBlocks(const glm::vec3& camPos,
                    oglwrap::LazyUniform<glm::ivec2>& Offset,
                    oglwrap::LazyUniform<int>& mipmapLevelUnif
    );
    void CreateConnectors(glm::ivec2 pos, glm::vec2 camPos);

public:
    size_t w, h;

    TerrainMesh(const std::string& terrainFile);

    void render(const glm::vec3& camPos,
                oglwrap::LazyUniform<glm::ivec2>& Offset,
                oglwrap::LazyUniform<int>& mipmapLevel
    );

    unsigned char fetchHeight(glm::ivec2 v) const;
    double getHeight(double x, double y) const;
};
