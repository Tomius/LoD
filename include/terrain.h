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
    oglwrap::LazyUniform<int> mipmapLevel;
    oglwrap::LazyUniformSampler heightMap, normalMap, grassMap, grassNormalMap;
    TerrainMesh mesh;

    Skybox& skybox;
public:
    int w, h;

    Terrain(Skybox& skybox);
    void reshape(const glm::mat4& projMat);
    glm::vec3 getScales() const;
    void render(float time, const glm::mat4& camMat, const glm::vec3& camPos);
    unsigned char fetchHeight(glm::ivec2 v) const;
    double getHeight(double x, double y) const;
};

#endif // header guard
