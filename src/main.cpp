#include <vector>
#include <string>
#include <iostream>
#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "oglwrap.hpp"
#include "skybox.h"
#include "camera.h"
#include "terrain.h"
#include "bloom.h"

void GL_Init(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void FPS_Display(float time) {
    static float accumTime = 0.0f;
    static float lastCall = time;
    float dt = time - lastCall;
    lastCall = time;
    static int calls = 0;

    calls++;
    accumTime += dt;
    if(accumTime > 1.0f){
        std::cout << "FPS: " << calls << std::endl;
        accumTime = calls = 0;
    }
}

int main() {
    bool fixMouse = false;
    sf::Window window(sf::VideoMode(800, 600), "Land of Dreams");
    window.setVerticalSyncEnabled(false); // bloom -> i want to draw twice per frame
    sf::Clock clock;
    if(glewInit() == GLEW_OK && GLEW_VERSION_3_3) try {
            GL_Init();
            Skybox skybox;
            Terrain terrain(skybox);
            BloomEffect bloom;
            Camera cam(glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(1.0f, 29.0f, 1.0f), 50.0f);

            sf::Event event;
            bool running = true;
            while(running) {

                while(window.pollEvent(event)) {
                    if(event.type == sf::Event::Closed ||
                        (event.type == sf::Event::KeyPressed &&
                         event.key.code == sf::Keyboard::Escape)) {
                        running = false;
                    } else if(event.type == sf::Event::Resized) {
                        int width = event.size.width, height = event.size.height;

                        glViewport(0, 0, width, height);
                        bloom.Reshape(width, height);
                        auto projMat = glm::perspectiveFov(
                          60.0f, float(width), float(height), 5.0f, 6000.0f
                        );
                        skybox.Reshape(projMat);
                        terrain.Reshape(projMat);

                    } else if(event.type == sf::Event::KeyPressed &&
                              event.key.code == sf::Keyboard::F11) {
                          fixMouse = !fixMouse;
                          window.setMouseCursorVisible(!fixMouse);
                    }
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                cam.Update(window, fixMouse);
                glm::mat4 camMatrix = cam.CameraMatrix();
                glm::vec3 camPos = cam.getPos();
                float time = clock.getElapsedTime().asSeconds();
                FPS_Display(time);

                skybox.Render(time, camMatrix);
                terrain.Render(time, camMatrix, camPos);
                bloom.Render();

                window.display();
            }

            return 0;

        } catch(std::exception& err) {
            std::cerr << err.what();
        }

    return 1;
}
