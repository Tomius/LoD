#ifndef HEADER_A891BD5159655394
#define HEADER_A891BD5159655394

#include "terrainMesh.h"
#include "skybox.h"
#include "shadow.h"

class Terrain {
    oglwrap::Program prog, shadow_prog;
    oglwrap::VertexShader vs, shadow_vs;
    oglwrap::FragmentShader fs, shadow_fs;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix;
    oglwrap::LazyUniform<glm::mat4> shadow_MVP, shadowBiasMVP;
    oglwrap::LazyUniform<glm::vec4> sunData;
    oglwrap::LazyUniform<glm::vec3> scales, shadow_scales;
    oglwrap::LazyUniform<glm::vec2> offset, shadow_offset;
    oglwrap::LazyUniformSampler heightMap, normalMap, colorMap;
    oglwrap::LazyUniformSampler shadow_heightMap, shadowMap;
    TerrainMesh mesh;

    Skybox& skybox;
public:
    Terrain(Skybox& skybox);
    void Reshape(const glm::mat4& projMat);
    void ShadowRender(float time, const glm::vec3& camPos, const Shadow& shadow);
    void Render(float time, const glm::mat4& camMat, const glm::vec3& camPos, const Shadow& shadow);
};

#endif // header guard
