#include "hills.h"
#include "misc.h"

using namespace oglplus;

Hills::Hills(const Skybox& skybox)
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

    FragmentShader fs;
    fs.Source(File2Str("terrain.frag")).Compile();

    prog << vs << fs << skybox.sky_fs;
    prog.Link();

    heightMap.Set(0);
    normalMap.Set(1);
    colorMap.Set(2);
}

void Hills::Reshape(const oglplus::Mat4f& projMat) {
    projectionMatrix.Set(projMat);
}

void Hills::Render(float time, const oglplus::Mat4f& camMat, const oglplus::Vec3f& camPos) {
    prog.Use();
    cameraMatrix.Set(camMat);
    sunData.Set(skybox.getSunData(time));
    mesh.Render(camPos, offset, scales);
}
