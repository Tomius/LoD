#ifndef HEADER_A891BD5159655394
#define HEADER_A891BD5159655394

#include "terrainMesh.h"
#include "skybox.h"
#include "shadow.h"

class Terrain {
    oglwrap::Program prog;
    oglwrap::VertexShader vs;
    oglwrap::FragmentShader fs;

    oglwrap::LazyUniform<glm::mat4> projectionMatrix, cameraMatrix;
    oglwrap::LazyUniform<glm::vec4> sunData;
    oglwrap::LazyUniform<glm::vec3> scales;
    oglwrap::LazyUniform<glm::ivec2> offset;
    oglwrap::LazyUniformSampler heightMap, normalMap, colorMap;
    TerrainMesh mesh;

    Skybox& skybox;
public:
    Terrain(Skybox& skybox);
    void Reshape(const glm::mat4& projMat);
    void Render(float time, const glm::mat4& camMat, const glm::vec3& camPos);
};

#endif // header guard
