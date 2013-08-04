#include "terrain.h"
#include "misc.h"

using namespace oglplus;

Terrain::Terrain(const Skybox& skybox)
    : projectionMatrix(prog, "ProjectionMatrix")
    , cameraMatrix(prog, "CameraMatrix")
    , sunData(prog, "SunData")
    , scales(prog, "Scales")
    , offset(prog, "Offset")
    , heightMap(prog, "HeightMap")
    , normalMap(prog, "NormalMap")
    , colorMap(prog, "ColorMap")
    , mesh("terrain/mideu.terrain",
           "terrain/mideu.rtc")
    , skybox(skybox) {

    VertexShader vs;
    vs.Source(File2Str("terrain.vert")).Compile();

    GeometryShader gs;
    gs.Source(File2Str("terrain.geom")).Compile();

    FragmentShader fs;
    fs.Source(File2Str("terrain.frag")).Compile();

    prog << vs << fs << skybox.sky_fs;
    prog.Link();

    heightMap.Set(0);
    normalMap.Set(1);
    colorMap.Set(2);
}

void Terrain::Reshape(const oglplus::Mat4f& projMat) {
    projectionMatrix.Set(projMat);
}

void Terrain::Render(float time, const oglplus::Mat4f& camMat, const oglplus::Vec3f& camPos) {
    prog.Use();
    cameraMatrix.Set(camMat);
    sunData.Set(skybox.getSunData(time));
    mesh.Render(camPos, offset, scales);
}
