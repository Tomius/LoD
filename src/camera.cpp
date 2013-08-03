#include <cmath>
#include "camera.h"

using namespace oglplus;
const float maxPitchAngle = 85./90. * M_PI_2;

Camera::Camera(Vec3f _pos, Vec3f _target, float _speedPerSec)
    : target(_pos)
    , fwd(Normalized(_target - _pos))
    , rotx(atan2(fwd.z(), fwd.x()))
    , roty(asin(fwd.y()))
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
    fwd = Vec3f(
              cos(roty) * cos(rotx),
              sin(roty),
              cos(roty) * sin(rotx)
          );
    Vec3f right = Normalized(Cross(fwd, Vec3f(0.0, 1.0f, 0.0)));

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        target += fwd * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        target -= fwd * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        target += right * ds;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        target -= right * ds;
}

CamMatrixf Camera::CameraMatrix() {
    return CamMatrixf::LookingAt(target - fwd, target);
}

ModelMatrixf Camera::ModelMatrix() {
    return ModelMatrixf::Translation(-target);
}

Vec3f Camera::getTarget() {
    return target;
}

Vec3f Camera::getPos() {
    return target  - fwd;
}
