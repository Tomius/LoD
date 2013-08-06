#include <cmath>
#include "camera.h"

using namespace oglwrap;
using namespace glm;

const float maxPitchAngle = 85./90. * M_PI_2;

Camera::Camera(const glm::vec3& _pos, const glm::vec3& _target, float _speedPerSec)
    : target(_pos)
    , fwd(normalize(_target - _pos))
    , rotx(atan2(fwd.z, fwd.x))
    , roty(asin(fwd.y))
    , speedPerSec(_speedPerSec) {

    assert(fabs(roty) < maxPitchAngle);
}

void Camera::Update(const sf::Window& window, bool fixMouse) {
    static sf::Clock clock;
    static float prevTime;
    float time = clock.getElapsedTime().asSeconds();
    float dt =  time - prevTime;
    prevTime = time;

    sf::Vector2i loc = sf::Mouse::getPosition(window);
    sf::Vector2i diff;
    if(fixMouse) {
        sf::Vector2i screenHalf = sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2);
        diff = loc - screenHalf;
        sf::Mouse::setPosition(screenHalf, window);
    } else {
        static sf::Vector2i prevLoc;
        diff = loc - prevLoc;
        prevLoc = loc;
    }

    static bool firstExec = true, lastFixMouse = fixMouse;
    if(firstExec || lastFixMouse != fixMouse) {
        firstExec = false;
        lastFixMouse = fixMouse;
        return;
    }

    // Mouse movement - update the coordinate system
    if(diff.x || diff.y) {
        rotx += diff.x * 0.0035f;
        roty += -diff.y * 0.0035f;

        if(fabs(roty) > maxPitchAngle)
            roty = roty/fabs(roty) * maxPitchAngle;
    }

    // WASD movement
    float ds = dt * speedPerSec;
    fwd = vec3(
              cos(roty) * cos(rotx),
              sin(roty),
              cos(roty) * sin(rotx)
          );
    vec3 right = normalize(cross(fwd, vec3(0.0, 1.0f, 0.0)));

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        target += fwd * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        target -= fwd * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        target += right * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        target -= right * ds;
}

glm::mat4 Camera::CameraMatrix() {
    return lookAt(target - fwd, target, vec3(0.0, 1.0f, 0.0));
}

glm::mat4 Camera::ModelMatrix() {
    return translate(mat4(1.0), target);
}

glm::vec3 Camera::getTarget() {
    return target;
}

glm::vec3 Camera::getPos() {
    return target - fwd;
}
