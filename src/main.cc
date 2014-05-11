#include <vector>
#include <string>
#include <iostream>
#include <SFML/Window.hpp>

#define OGLWRAP_INSTATIATE 1

#include "oglwrap_config.h"

#include "engine/timer.h"
#include "engine/scene.h"
#include "engine/camera.h"
#include "engine/gameobject.h"

#include "./charmove.h"
#include "./skybox.h"
#include "./terrain.h"
#include "./bloom.h"
#include "./ayumi.h"
#include "./tree.h"
#include "./shadow.h"
#include "./map.h"

using namespace oglwrap;
Context gl;

extern const float GRAVITY = 18.0f;
/* 0 -> max quality
   2 -> max performance */
extern const int PERFORMANCE = 1;
extern const float kFieldOfView = 60;
double ogl_version;
bool was_left_click = false;

void glInit() {
  gl.ClearColor(1.0f, 0.0f, 0.0f, 0.0f);
  gl.ClearDepth(1.0f);
  gl.Enable(Capability::DepthTest);
  gl.Enable(Capability::DepthClamp);
  gl.CullFace(Face::Back);
}

void FpsDisplay() {
  static sf::Clock clock;
  float time = clock.getElapsedTime().asSeconds();

  static float accum_time = 0.0f;
  static float last_call = time;
  float dt = time - last_call;
  last_call = time;
  static int calls = 0;

  calls++;
  accum_time += dt;
  if (accum_time > 1.0f) {
    std::cout << "FPS: " << calls << std::endl;
    accum_time = calls = 0;
  }
}

static sf::Clock dbg_clock;

static void PrintDebugText(const std::string& str) {
  std::cout << str << ": ";
}

static void PrintDebugTime() {
  std::cout << dbg_clock.getElapsedTime().asMilliseconds() << " ms" << std::endl;
  dbg_clock.restart();
}

int main() {
  PrintDebugText("Creating the OpenGL context");
  sf::Window window(
    sf::VideoMode(800, 600),
    "Land of Dreams",
    sf::Style::Default,
    sf::ContextSettings(32, 0, 0, 2, 1)
  );
  PrintDebugTime();

  sf::ContextSettings settings = window.getSettings();
  std::cout << " - Depth bits: " << settings.depthBits << std::endl;
  std::cout << " - Stencil bits: " << settings.stencilBits << std::endl;
  std::cout << " - Anti-aliasing level: " << settings.antialiasingLevel << std::endl;
  std::cout << " - OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;

  ogl_version = settings.majorVersion + settings.minorVersion/10.0;

  if (ogl_version < 2.1) {
    std::cout << "At least OpenGL version 2.1 is required to run this program" << std::endl;
    return -1;
  }

  // No V-sync needed because of multiple draw calls per frame.
  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(false);

  engine::Scene scene;
  bool fixMouse = false;

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cout << "GlewInit error: " << glewGetErrorString(err) << std::endl;
    return -1;
  }

  try {
      glInit();

      dbg_clock.restart();
      PrintDebugText("Initializing the skybox");
        Skybox *skybox = scene.addSkybox(new Skybox{});
      PrintDebugTime();

      PrintDebugText("Initializing the shadow maps");
        Shadow *shadow = scene.addShadow(
          new Shadow{PERFORMANCE < 2 ? 512 : 256, 8, 8}
        );
      PrintDebugTime();

      PrintDebugText("Initializing the terrain");
        Terrain *terrain = scene.addGameObject(new Terrain{skybox, shadow});
        auto terrain_height =
          [terrain](double x, double y) {return terrain->getHeight(x, y);};
      PrintDebugTime();

      PrintDebugText("Initializing the trees");
        scene.addGameObject(new Tree{*terrain, skybox, shadow});
      PrintDebugTime();

      PrintDebugText("Initializing Ayumi");
        Ayumi *ayumi = scene.addGameObject(
          new Ayumi{skybox, shadow}
        );

        ayumi->addRigidBody(terrain_height, ayumi->transform.pos().y);

        CharacterMovement charmove {
          ayumi->transform, *ayumi->rigid_body
        };

        ayumi->charmove(&charmove);
      PrintDebugTime();

      charmove.setAnimation(&ayumi->getAnimation());

      engine::Transform& cam_offset =
        scene.addGameObject(new engine::GameObject{})->transform;
      ayumi->transform.addChild(cam_offset);
      cam_offset.localPos(ayumi->getMesh().bSphereCenter());

      engine::ThirdPersonalCamera cam(
        cam_offset, cam_offset.pos() + glm::vec3(ayumi->getMesh().bSphereRadius() * 2),
        terrain_height, 1.5f
      );

      charmove.setCamera(&cam);
      scene.addCamera(&cam);

      PrintDebugText("Initializing the resources for the bloom effect");
        scene.addAfterEffect(new BloomEffect{});
      PrintDebugTime();

      PrintDebugText("Initializing the map");
        Map *map = scene.addAfterEffect(new Map{glm::vec2(terrain->w, terrain->h)});
      PrintDebugTime();

      std::cout << "\nStarting the main loop.\n" << std::endl;

      sf::Event event;
      bool running = true;
      while(running) {

        while(window.pollEvent(event)) {
          switch(event.type) {
            case sf::Event::Closed:
              running = false;
              break;
            case sf::Event::KeyPressed:
              if (event.key.code == sf::Keyboard::Escape) {
                running = false;
              } else if (event.key.code == sf::Keyboard::F11) {
                fixMouse = !fixMouse;
                window.setMouseCursorVisible(!fixMouse);
              } else if (event.key.code == sf::Keyboard::F1) {
                scene.game_time_.toggle();
              } else if (event.key.code == sf::Keyboard::F2) {
                scene.environment_time_.toggle();
              } else if (event.key.code == sf::Keyboard::Space) {
                charmove.handleSpacePressed();
              } else if (event.key.code == sf::Keyboard::M) {
                map->toggle();
              }
              break;
            case sf::Event::Resized: {
                int width = event.size.width, height = event.size.height;

                gl.Viewport(width, height);

                auto projMat = glm::perspectiveFov<float>(
                  kFieldOfView, width, height, 0.5, 3000
                );

                scene.screenResized(projMat, width, height);
              }
              break;
            case sf::Event::MouseWheelMoved:
              cam.scrolling(event.mouseWheel.delta);
              break;
            case sf::Event::MouseButtonPressed:
              if (event.mouseButton.button == sf::Mouse::Left) {
                was_left_click = true;
              }
              break;
            default:
              break;
          }
        }

        gl.Clear().Color().Depth();

        FpsDisplay();

        scene.turn();

        window.display();

      }

      return 0;

    } catch(std::exception& err) {
      std::cerr << err.what();
    }

  return 1;
}
