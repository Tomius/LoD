#ifndef HEADER_A71A3471183A48FA
#define HEADER_A71A3471183A48FA

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "oglplus/all.hpp"
#include "oglplus/bound/texture.hpp"
#include "terrainData.h"

#define mmLev 6
// BlockSize is actually half the blocks' size
// Also thanks to hexagonal tessellation, we have fractal coordinates
// so at mmLev 0, the texture would actually get magnified, which we
// really don't want as it creates really weird artifacts. So mipmaps
// actually start from 1, not zero.
const int blockSize = 1 << mmLev;

class TerrainMesh {
    oglplus::Context gl;
    oglplus::VertexArray vao[mmLev];
    oglplus::Buffer positions[mmLev], indices[mmLev];
    oglplus::Buffer borderIndices[mmLev][6][2];
    size_t vertexNum[mmLev], indexNum[mmLev];
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
