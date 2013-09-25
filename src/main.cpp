#include <vector>
#include <string>
#include <iostream>
#include <SFML/Window.hpp>

#if defined(__APPLE__)
#include <OpenGL/glew.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>
#endif

#define OGLWRAP_DEBUG 1
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/utils/camera.hpp"

#include "charmove.hpp"
#include "skybox.h"
#include "terrain.h"
#include "bloom.h"
#include "shadow.h"
#include "ayumi.h"
#include "tree.hpp"

/* Todo:
      - horizontal collision.
      - enemies (zombies?)
      - attacks.
*/

using namespace oglwrap;

void GL_Init() {
  gl(ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
  gl(ClearDepth(1.0f));
  gl(Enable(GL_DEPTH_TEST));
  gl(Enable(GL_FRAMEBUFFER_SRGB));
}

void FPS_Display(float time) {
  static float accumTime = 0.0f;
  static float lastCall = time;
  float dt = time - lastCall;
  lastCall = time;
  static int calls = 0;

  calls++;
  accumTime += dt;
  if(accumTime > 1.0f) {
    std::cout << "FPS: " << calls << std::endl;
    accumTime = calls = 0;
  }
}

int main() {
  sf::Window window(
    sf::VideoMode(800, 600),
    "Land of Dreams",
    sf::Style::Default,
    sf::ContextSettings(32, 0, 4, 3, 1)
  );
  window.setVerticalSyncEnabled(false); // bloom -> i want to draw twice per frame

  sf::Clock clock;
  bool fixMouse = false;
  if(glewInit() == GLEW_OK && GLEW_VERSION_3_1) try {
      GL_Init();
      Skybox skybox;
      Ayumi ayumi(skybox);
      Terrain terrain(skybox);
      Tree tree(skybox, terrain);
      BloomEffect bloom;
      CharacterMovement charmove(glm::vec3(0, terrain.getScales().y * 13, 0));
      ThirdPersonalCamera cam(
        ayumi.mesh.boundingSphere_Center() + glm::vec3(ayumi.mesh.boundingSphere_Radius() * 3),
        ayumi.mesh.boundingSphere_Center(),
        1.5f
      );

      sf::Event event;
      bool running = true;
      while(running) {

        while(window.pollEvent(event)) {
          switch(event.type) {
            case sf::Event::Closed:
              running = false;
              break;
            case sf::Event::KeyPressed:
              if(event.key.code == sf::Keyboard::Escape) {
                running = false;
              } else if(event.key.code == sf::Keyboard::F11) {
                fixMouse = !fixMouse;
                window.setMouseCursorVisible(!fixMouse);
              }
              break;
            case sf::Event::Resized: {
                int width = event.size.width, height = event.size.height;

                glViewport(0, 0, width, height);
                bloom.reshape(width, height);

                auto projMat = glm::perspectiveFov<float>(
                                 60, width, height, 1, 3000
                               );

                ayumi.reshape(projMat);
                skybox.reshape(projMat);
                terrain.reshape(projMat);
                tree.reshape(projMat);
              }
              break;
            case sf::Event::MouseWheelMoved:
              cam.scrolling(event.mouseWheel.delta);
              break;
            default:
              break;
          }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = clock.getElapsedTime().asSeconds();

        cam.updateRotation(window, fixMouse);
        charmove.update(cam, ayumi.mesh.offset_since_last_frame());
        cam.updateTarget(charmove.getPos() + ayumi.mesh.boundingSphere_Center());
        auto scales = terrain.getScales();
        charmove.updateHeight(
          scales.y * terrain.getHeight(
            charmove.getPos().x / (double)scales.x,
            charmove.getPos().z / (double)scales.z
          )
        );

        glm::mat4 camMatrix = cam.cameraMatrix();
        glm::vec3 camPos = cam.getPos();
        FPS_Display(time);

        skybox.render(time, camMatrix);
        terrain.render(time, camMatrix, camPos);
        ayumi.render(time, cam, charmove);
        tree.render(time, cam);
        bloom.render();

        window.display();

      }

      return 0;

    } catch(std::exception& err) {
      std::cerr << err.what();
    }

  return 1;
}
