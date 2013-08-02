#ifndef HEADER_A71A3471183A48FA
#define HEADER_A71A3471183A48FA

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <oglplus/all.hpp>
#include <oglplus/bound/texture.hpp>
#include "terrainData.h"

#define mmLev 6
const int blockSize = 1 << mmLev; // Actually it's half the blocks' size

class Terrain {
    oglplus::Context gl;
    oglplus::VertexArray vao[mmLev];
    oglplus::Buffer positions[mmLev], indices[mmLev];
    size_t vertexNum[mmLev], indexNum[mmLev];
    oglplus::Texture colorMap, heightMap, normalMap;

    TerrainData terrain;
    size_t w, h;
    RawImageData image;

    void DrawBlocks(const oglplus::Vec3f& camPos,
                    oglplus::LazyProgramUniform<oglplus::Vec2f>& Offset
    );
public:
    Terrain(const std::string& terrainFile,
            const std::string& diffusePicture
    );
    void Render(const oglplus::Vec3f& camPos,
                oglplus::LazyProgramUniform<oglplus::Vec2f>& Offset,
                oglplus::LazyProgramUniform<oglplus::Vec3f>& scales
    );
};


#endif // header guard
