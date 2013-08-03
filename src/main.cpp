#include <vector>
#include <string>
#include <iostream>
#include <SFML/Window.hpp>
#include <GL/glew.h>
// Release mode + these might get you +5 fps
//#define OGLPLUS_LOW_PROFILE 1
//#define OGLPLUS_CHECK(x)
//#define assert(x)
#include "oglplus/gl.hpp"
#include "oglplus/all.hpp"
#include "skybox.h"
#include "camera.h"
#include "terrain.h"
#include "bloom.h"

oglplus::Context gl;

void GL_Init(){
    gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl.ClearDepth(1.0f);
    gl.Enable(oglplus::Capability::DepthTest);
    gl.Enable(oglplus::Capability::FramebufferSrgb);
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

#include "terrainData.h"
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
            Camera cam(oglplus::Vec3f(0.0, 30.0, 0.0), oglplus::Vec3f(1.0, 29.0, 1.0), 50.0f);

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

                        gl.Viewport(width, height);
                        bloom.Reshape(width, height);
                        auto projMat = oglplus::CamMatrixf::PerspectiveX(
                          oglplus::Degrees(60),
                          float(width) / height,
                          10, 6000
                        );
                        skybox.Reshape(projMat);
                        terrain.Reshape(projMat);

                    } else if(event.type == sf::Event::KeyPressed &&
                              event.key.code == sf::Keyboard::F11) {
                          fixMouse = !fixMouse;
                          window.setMouseCursorVisible(!fixMouse);
                    }
                }

                gl.Clear().ColorBuffer().DepthBuffer();

                cam.Update(window, fixMouse);
                oglplus::Mat4f camMatrix = cam.CameraMatrix();
                oglplus::Vec3f camPos = cam.getPos();
                float time = clock.getElapsedTime().asSeconds();
                FPS_Display(time);

                skybox.Render(time, camMatrix);
                terrain.Render(time, camMatrix, camPos);
                bloom.Render();

                window.display();
            }

            return 0;

        } catch(oglplus::ProgramBuildError& err) {
            using namespace std;
            cerr <<
                 "Error:          " << err.what() << endl <<
                 "Function:       " << err.Func() << endl <<
                 "Symbol:         " << err.GLSymbol() << endl <<
                 "Class:          " << err.ClassName() << (err.ObjectDescription().empty() ? "" :
                         " - '" + err.ObjectDescription() + "'") << endl <<
                 "Bind Target:    " << err.BindTarget() << endl <<
                 "File:           [" << err.File() << ":" << err.Line() << "]" <<
                 endl << endl << err.Log() << endl;
            err.Cleanup();
        } catch(oglplus::Error& err) {
            using namespace std;
            cerr <<
                 "Error:          " << err.what() << endl <<
                 "Function:       " << err.Func() << endl <<
                 "Symbol:         " << err.GLSymbol() << endl <<
                 "Class:          " << err.ClassName() << (err.ObjectDescription().empty() ? "" :
                         " - '" + err.ObjectDescription() + "'") << endl <<
                 "Bind Target:    " << err.BindTarget() << endl <<
                 "File:           [" << err.File() << ":" << err.Line() << "]" << endl;
            err.Cleanup();
        }
    return 1;
}
