#ifndef HEADER_19548EFADB46803A
#define HEADER_19548EFADB46803A

#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "oglplus/gl.hpp"
#include "oglplus/all.hpp"

class Camera{
    oglplus::Vec3f target, fwd;
    float rotx, roty; // rot angle relative to pos z axis
    const float speedPerSec;
public:
    Camera(oglplus::Vec3f pos, oglplus::Vec3f target = oglplus::Vec3f(), float speedPerSec = 5.0f);
    void Update(const sf::Window& window, bool fixMouse = false);
    oglplus::CamMatrixf CameraMatrix();
    oglplus::ModelMatrixf ModelMatrix();
    oglplus::Vec3f getTarget();
    oglplus::Vec3f getPos();
};


#endif // header guard
