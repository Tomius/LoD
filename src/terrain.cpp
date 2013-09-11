#include "terrain.h"

using namespace oglwrap;

static const glm::vec3 scale_vector(3, 2, 3);

Terrain::Terrain(Skybox& skybox)
    : vs("terrain.vert")
    , fs("terrain.frag")
    , projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , sunData(prog, "SunData")
    , scales(prog, "Scales")
    , offset(prog, "Offset")
    , mipmapLevel(prog, "MipmapLevel")
    , heightMap(prog, "HeightMap")
    , normalMap(prog, "NormalMap")
    , colorMap(prog, "ColorMap")
    , grassMap(prog, "GrassMap")
    , grassNormalMap(prog, "GrassNormalMap")
    , mesh("terrain/mideu.rtd",
           "terrain/mideu.rtc")
    , skybox(skybox) {

    prog << vs << fs << skybox.sky_fs;
    prog.link().use();

    heightMap.set(0);
    colorMap.set(1);
    grassMap.set(2);
    grassNormalMap.set(3);

    scales = scale_vector;
}

glm::vec3 Terrain::getScales() const {
    return scale_vector;
}

void Terrain::reshape(const glm::mat4& projMat) {
    prog.use();
    projectionMatrix = projMat;
}

void Terrain::render(float time, const glm::mat4& camMat, const glm::vec3& camPos) {
    prog.use();
    cameraMatrix.set(camMat);
    sunData.set(skybox.getSunData(time));
    mesh.render(camPos, offset, mipmapLevel);
}

float Terrain::getHeight(float x, float y) {
    return mesh.getHeight(x, y);
}
