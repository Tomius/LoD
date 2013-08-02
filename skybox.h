#ifndef HEADER_16753D7B23630095
#define HEADER_16753D7B23630095

#include <GL/glew.h>
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/shapes/wrapper.hpp>

class Skybox {
    oglplus::Context gl;

    oglplus::shapes::ShapeWrapper skybox;

    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;

    oglplus::LazyProgramUniform<oglplus::Mat4f> projectionMatrix, cameraMatrix;
    oglplus::LazyProgramUniform<oglplus::Vec4f> sunData;
public:
    Skybox();
    void Reshape(const oglplus::CamMatrixf& projMat);
    void Render(float time, const oglplus::CamMatrixf& camMatrix);

    oglplus::Texture envMap;
    oglplus::FragmentShader sky_fs;
    oglplus::Vec4f getSunData(float time) const;
};


#endif // header guard
