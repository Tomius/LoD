#include "terrain.h"

using namespace oglwrap;

Terrain::Terrain(Skybox& skybox)
    : vs("terrain.vert")
    , shadow_vs("terrain_shadow.vert")
    , fs("terrain.frag")
    , shadow_fs("shadow.frag")
    , projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , shadow_MVP(shadow_prog, "MVP")
    , shadowBiasMVP(prog, "ShadowMVP")
    , sunData(prog, "SunData")
    , scales(prog, "Scales")
    , shadow_scales(shadow_prog, "Scales")
    , offset(prog, "Offset")
    , shadow_offset(shadow_prog, "Offset")
    , heightMap(prog, "HeightMap")
    , normalMap(prog, "NormalMap")
    , colorMap(prog, "ColorMap")
    , shadow_heightMap(shadow_prog, "HeightMap")
    , shadowMap(prog, "ShadowMap")
    , mesh("terrain/mideu.terrain",
           "terrain/mideu.rtc")
    , skybox(skybox) {

    prog << vs << fs << skybox.sky_fs;
    prog.Link().Use();

    heightMap.Set(0);
    normalMap.Set(1);
    colorMap.Set(2);
    shadowMap.Set(3);

    shadow_prog << shadow_vs << shadow_fs;
    shadow_prog.Link().Use();

    shadow_heightMap.Set(0);
}

void Terrain::Reshape(const glm::mat4& projMat) {
    projectionMatrix = projMat;
}

void Terrain::ShadowRender(float time, const glm::vec3& camPos, const Shadow& shadow) {
    shadow_prog.Use();
    shadow_MVP = shadow.MVP(skybox.SunPos(time));
    mesh.Render(camPos, shadow_offset, shadow_scales);
}

void Terrain::Render(float time, const glm::mat4& camMat, const glm::vec3& camPos, const Shadow& shadow) {
    prog.Use();
    cameraMatrix.Set(camMat);
    sunData.Set(skybox.getSunData(time));
    shadowBiasMVP = shadow.BiasMVP(skybox.SunPos(time));
    shadow.ShadowTex().Active(3);
    shadow.ShadowTex().Bind();
    mesh.Render(camPos, offset, scales);
}
