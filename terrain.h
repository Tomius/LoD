#ifndef HEADER_A891BD5159655394
#define HEADER_A891BD5159655394

#include "terrainMesh.h"
#include "skybox.h"

class Terrain {
    oglplus::Context gl;
    oglplus::Program prog;
    oglplus::LazyProgramUniform<oglplus::Mat4f> projectionMatrix, cameraMatrix;
    oglplus::LazyProgramUniform<oglplus::Vec4f> sunData;
    oglplus::LazyProgramUniform<oglplus::Vec3f> scales;
    oglplus::LazyProgramUniform<oglplus::Vec2f> offset;
    oglplus::LazyProgramUniformSampler heightMap, normalMap, colorMap;
    TerrainMesh mesh;

    const Skybox& skybox;
public:
    Terrain(const Skybox& skybox);
    void Reshape(const oglplus::Mat4f& projMat);
    void Render(float time, const oglplus::Mat4f& camMat, const oglplus::Vec3f& target);
};

#endif // header guard
