/** @file charmove.h
    @brief Implements features related to character movement.
*/

#ifndef HEADER_326DECDF70F7C91
#define HEADER_326DECDF70F7C91

#pragma once

#include "oglwrap/utils/camera.hpp"

namespace oglwrap {

class CharacterMovement {
    glm::vec3 pos;

    // Current and destination rotation angles.
    double currRot, destRot;

    // Moving speed per second in OpenGL units.
    float rotSpeed;

    bool walking, transition;

public:
    CharacterMovement(glm::vec3 pos = glm::vec3(),
                      float rotationSpeed_PerSec = 180.0f)
        : pos(pos)
        , currRot(0)
        , destRot(0)
        , rotSpeed(rotationSpeed_PerSec)
        , walking(false)
        , transition(false)
    { }

    void update(const Camera& cam, glm::vec3 characterOffset) {
        using namespace glm;

        static sf::Clock clock;
        static float prevTime = 0;
        float time = clock.getElapsedTime().asSeconds();
        float dt =  time - prevTime;
        prevTime = time;

        ivec2 moveDir; // up and right is positive
        bool w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        bool a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        bool s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        bool d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        if(w && !s){
            moveDir.y = 1;
        } else if(s && !w) {
            moveDir.y = -1;
        }

        if(d && !a) {
            moveDir.x = 1;
        } else if(a && !d) {
            moveDir.x = -1;
        }

        static ivec2 lastMoveDir;
        bool lastWalking = walking;
        walking = moveDir.x || moveDir.y;
        transition = transition || (walking != lastWalking) || (lastMoveDir != moveDir);
        lastMoveDir = moveDir;

        if(walking) {
            double cameraRot = -cam.getRoty(); // -z is forward
            double moveRot = atan2(moveDir.y, moveDir.x); // +y is forward
            destRot = ToDegree(cameraRot + moveRot);
            destRot = fmod(destRot, 360);

            double diff = destRot - currRot;
            double sign = diff / fabs(diff);

            // Take the shorter path.
            while(fabs(diff) > 180) {
                destRot -= sign * 360;

                diff = destRot - currRot;
                sign = diff / fabs(diff);
            }

           if(transition) {
                // an angle difference of 1 degree isn't noticeable, but without this,
                // the character would continuously vibrate right to left.
                if(fabs(diff) > 1) {
                    currRot += sign * dt * rotSpeed;
                    currRot = fmod(currRot, 360);
                } else {
                    transition = false;
                }
           } else {
                currRot = fmod(destRot, 360);
           }
        }

        pos += mat3(rotate(mat4(), (float)fmod(currRot, 360), vec3(0,1,0))) * characterOffset;
    }

    void updateHeight(float groundHeight) {
        const float diff = groundHeight - pos.y;
        const float offs = std::max(fabs(diff) / 5.0, 0.05);
        if(fabs(diff) > offs)
            pos.y += diff / fabs(diff) * offs;
    }


    bool isWalking() const {
        return walking;
    }

    // FIXME: pos.y
    glm::mat4 getModelMatrix() const {
        using namespace glm;
        glm::mat4 tXZ = glm::translate(glm::mat4(), glm::vec3(pos.x, 0, pos.z));
        glm::mat4 tY = glm::translate(glm::mat4(), glm::vec3(0, pos.y, 0));
        glm::mat4 rot = glm::rotate(glm::mat4(), (float)fmod(currRot, 360), vec3(0,1,0));
        return tXZ * rot * tY;
    }

    glm::vec3 getPos() const {
        return pos;
    }
};

}
#endif // header guard
