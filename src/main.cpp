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

#include "oglwrap/oglwrap.hpp"
#include "oglwrap/utils/camera.hpp"

#include "charmove.hpp"
#include "skybox.hpp"
#include "terrain.hpp"
#include "bloom.hpp"
#include "ayumi.hpp"
#include "tree.hpp"
#include "shadow.hpp"

using namespace oglwrap;

const float GRAVITY = 0.6f;

void GL_Init() {
  gl(ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
  gl(ClearDepth(1.0f));
  gl(Enable(GL_DEPTH_TEST));
  gl(Enable(GL_DEPTH_CLAMP));
  gl(Enable(GL_FRAMEBUFFER_SRGB));
  gl(CullFace(GL_BACK));
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
    sf::ContextSettings(24, 8, 0, 3, 2)
  );
  window.setVerticalSyncEnabled(false); // bloom -> i want to draw twice per frame

  sf::Clock clock;
  bool fixMouse = false;
  if(glewInit() == GLEW_OK && GLEW_VERSION_3_2) try {
      GL_Init();
      Skybox skybox;
      BloomEffect bloom;
      Terrain terrain(skybox);
      Shadow shadow;
      Tree tree(skybox, terrain);
      CharacterMovement charmove(glm::vec3(0, terrain.getScales().y * 13, 0));

      Ayumi ayumi(skybox, charmove);

      ThirdPersonalCamera cam(
        ayumi.getMesh().bSphereCenter() + glm::vec3(ayumi.getMesh().bSphereRadius() * 2),
        ayumi.getMesh().bSphereCenter(),
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
                bloom.resize(width, height);
                shadow.resize(width, height);

                auto projMat = glm::perspectiveFov<float>(60, width, height, 0.5, 3000);

                ayumi.resize(projMat);
                skybox.resize(projMat);
                terrain.resize(projMat);
                tree.resize(projMat);
              }
              break;
            case sf::Event::MouseWheelMoved:
              cam.scrolling(event.mouseWheel.delta);
              break;
            default:
              break;
          }
        }

        gl( Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

        float time = clock.getElapsedTime().asSeconds();

        // Updates
        cam.updateRotation(window, fixMouse);
        charmove.update(cam, ayumi.getMesh().offsetSinceLastFrame());
        cam.updateTarget(charmove.getPos() + ayumi.getMesh().bSphereCenter());
        auto scales = terrain.getScales();
        charmove.updateHeight(
          scales.y * terrain.getHeight(
            charmove.getPos().x / (double)scales.x,
            charmove.getPos().z / (double)scales.z
          )
        );
        ayumi.updateStatus(time, charmove);

        glm::mat4 camMatrix = cam.cameraMatrix();
        glm::vec3 camPos = cam.getPos();
        FPS_Display(time);

        shadow.startShadowRender();
        ayumi.shadowRender(time, shadow, charmove);
        shadow.endShadowRender();

        skybox.render(time, camMatrix);
        terrain.render(time, camMatrix, camPos);
        ayumi.render(time, shadow, cam, charmove);
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
