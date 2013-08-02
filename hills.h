#ifndef HEADER_A891BD5159655394
#define HEADER_A891BD5159655394

#include "terrain.h"
#include "skybox.h"

class Hills {
    oglplus::Context gl;
    oglplus::Program prog;
    oglplus::LazyProgramUniform<oglplus::Mat4f> projectionMatrix, cameraMatrix;
    oglplus::LazyProgramUniform<oglplus::Vec4f> sunData;
    oglplus::LazyProgramUniform<oglplus::Vec3f> scales;
    oglplus::LazyProgramUniform<oglplus::Vec2f> offset;
    oglplus::LazyProgramUniformSampler heightMap, normalMap, colorMap;
    Terrain mesh;

    const Skybox& skybox;
public:
    Hills(const Skybox& skybox);
    void Reshape(const oglplus::Mat4f& projMat);
    void Render(float time, const oglplus::Mat4f& camMat, const oglplus::Vec3f& target);
};

#endif // header guard
