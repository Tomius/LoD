#ifndef HEADER_A71A3471183A48FA
#define HEADER_A71A3471183A48FA

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "oglplus/all.hpp"
#include "oglplus/bound/texture.hpp"
#include "terrainData.h"

// Selecting the Block's size (is 2 ^ (blockMipmapLevel + 1)) is really essential
// but not trivial. Selecting the good number can prove you about twice as much
// performance as with selecting a number 1 higher or 1 lower number, and it won't
// just be faster, but it will also look better. So everytime you try a new mesh,
// tweak this value, the optimum should be near ceil(log2(texSize) / 2).

#define blockMipmapLevel 7
const int blockRadius = 1 << blockMipmapLevel;

class TerrainMesh {
    oglplus::Context gl;
    oglplus::VertexArray vao[blockMipmapLevel];
    oglplus::Buffer positions[blockMipmapLevel], indices[blockMipmapLevel];
    // Hexagons at the change of mipmap levels has to have irregular edges
    // or geometry cracks will happen. These edges could be drawn separately
    // if the PCIE wasn't already the bottleneck. So unfortunately we have to
    // have a fckload of IB that cover the full hexagon for every single possibility.
    // Actually not all the blockMipmapLevel * 314, we can use geometry to greatly reduce the number
    // of them. For example we know that a hexagon will have either exactly 6, or maximum 3
    // irregular edges so already halved the number of IBs we need. Also the irregular
    // edges are next to each other all the time, so that we only have to store let's say
    // the rightmost irregular edge's line ID, so now we have blockMipmapLevel * (1 + 6 + 6 + 6 + 1)
    // so blockMipmapLevel * 20 IB. And we also know that there won't be higher mipmap level than
    // the highest mipmap level we have (genius idea isn't it ?) so don't need irregular
    // edges for that mipmap level. So we actually need 20 * (blockMipmapLevel - 1) + 1 IB.
    oglplus::Buffer borderIndices[blockMipmapLevel][6][2];
    size_t vertexNum[blockMipmapLevel], indexNum[blockMipmapLevel];
    oglplus::Texture colorMap, heightMap, normalMap;

    TerrainData terrain;
    size_t w, h;
    RawImageData image;

    void DrawBlocks(const oglplus::Vec3f& camPos,
                    oglplus::LazyProgramUniform<oglplus::Vec2f>& Offset
    );
    void CreateConnectors(oglplus::Vec2f& pos, oglplus::Vec2f& camPos);
public:
    TerrainMesh(const std::string& terrainFile,
                const std::string& diffusePicture
    );
    void Render(const oglplus::Vec3f& camPos,
                oglplus::LazyProgramUniform<oglplus::Vec2f>& Offset,
                oglplus::LazyProgramUniform<oglplus::Vec3f>& scales
    );
};


#endif // header guard
