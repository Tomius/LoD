#ifndef HEADER_19548EFADB46803A
#define HEADER_19548EFADB46803A

#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "oglwrap/oglwrap.hpp"

class Camera{
    glm::vec3 target, fwd;
    float rotx, roty; // rot angle relative to pos z axis
    const float speedPerSec;
public:
    Camera(const glm::vec3& pos, const glm::vec3& target = glm::vec3(), float speedPerSec = 5.0f);
    void Update(const sf::Window& window, bool fixMouse = false);
    glm::mat4 CameraMatrix();
    glm::mat4 ModelMatrix();
    glm::vec3 getTarget();
    glm::vec3 getPos();
};


#endif // header guard
